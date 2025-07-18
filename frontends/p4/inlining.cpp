/*
Copyright 2013-present Barefoot Networks, Inc.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#include "inlining.h"

#include "frontends/common/resolveReferences/resolveReferences.h"
#include "frontends/p4/callGraph.h"
#include "frontends/p4/def_use.h"
#include "frontends/p4/methodInstance.h"
#include "frontends/p4/moveDeclarations.h"
#include "frontends/p4/parameterSubstitution.h"
#include "frontends/p4/resetHeaders.h"
#include "frontends/p4/toP4/toP4.h"
#include "frontends/p4/typeChecking/typeChecker.h"
#include "ir/annotations.h"
#include "lib/nullstream.h"

namespace P4 {

using namespace literals;

namespace {

class FindLocationSets : public Inspector {
    StorageMap *storageMap;
    std::map<const IR::Expression *, const LocationSet *> loc;

    const LocationSet *get(const IR::Expression *expression) const {
        auto result = ::P4::get(loc, expression);
        BUG_CHECK(result != nullptr, "No location set known for %1%", expression);
        return result;
    }
    void set(const IR::Expression *expression, const LocationSet *ls) {
        CHECK_NULL(expression);
        CHECK_NULL(ls);
        loc.emplace(expression, ls);
    }

 public:
    FindLocationSets(ReferenceMap *refMap, TypeMap *typeMap)
        : storageMap(new StorageMap(refMap, typeMap)) {}

    // default behavior
    bool preorder(const IR::Expression *expression) {
        set(expression, LocationSet::empty);
        return false;
    }

    bool preorder(const IR::AbstractSlice *expression) {
        visit(expression->e0);
        auto base = get(expression->e0);
        set(expression, base);
        return false;
    }

    bool preorder(const IR::TypeNameExpression *expression) {
        set(expression, LocationSet::empty);
        return false;
    }

    bool preorder(const IR::PathExpression *expression) {
        auto decl = storageMap->refMap->getDeclaration(expression->path, true);
        auto storage = storageMap->getStorage(decl);
        const LocationSet *result;
        if (storage != nullptr)
            result = new LocationSet(storage);
        else
            result = LocationSet::empty;
        set(expression, result);
        return false;
    }

    bool preorder(const IR::Member *expression) {
        visit(expression->expr);
        auto type = storageMap->typeMap->getType(expression, true);
        if (type->is<IR::Type_Method>()) return false;
        auto storage = get(expression->expr);

        auto basetype = storageMap->typeMap->getType(expression->expr, true);
        if (basetype->is<IR::Type_Array>()) {
            if (expression->member.name == IR::Type_Array::next ||
                expression->member.name == IR::Type_Array::last) {
                set(expression, storage);
                return false;
            }
        }

        auto fields = storage->getField(expression->member);
        set(expression, fields);
        return false;
    }

    bool preorder(const IR::ArrayIndex *expression) {
        visit(expression->left);
        visit(expression->right);
        auto storage = get(expression->left);
        if (expression->right->is<IR::Constant>()) {
            auto cst = expression->right->to<IR::Constant>();
            auto index = cst->asInt();
            auto result = storage->getIndex(index);
            set(expression, result);
        } else {
            set(expression, storage->allElements());
        }
        return false;
    }

    bool preorder(const IR::Operation_Binary *expression) {
        visit(expression->left);
        visit(expression->right);
        auto l = get(expression->left);
        auto r = get(expression->right);
        auto result = l->join(r);
        set(expression, result);
        return false;
    }

    bool preorder(const IR::Mux *expression) {
        visit(expression->e0);
        visit(expression->e1);
        visit(expression->e2);
        auto e0 = get(expression->e0);
        auto e1 = get(expression->e1);
        auto e2 = get(expression->e2);
        auto result = e0->join(e1)->join(e2);
        set(expression, result);
        return false;
    }

    bool preorder(const IR::ListExpression *expression) {
        visit(expression->components, "components");
        auto l = LocationSet::empty;
        for (auto c : expression->components) {
            auto cl = get(c);
            l = l->join(cl);
        }
        set(expression, l);
        return false;
    }

    bool preorder(const IR::Operation_Unary *expression) {
        visit(expression->expr);
        auto result = get(expression->expr);
        set(expression, result);
        return false;
    }

    const LocationSet *locations(const IR::Expression *expression) {
        (void)expression->apply(*this);
        auto ls = get(expression);
        if (ls != nullptr) return ls->canonicalize();
        return nullptr;
    }
};

/**
This class computes new names for inlined objects.
An object's name is prefixed with the instance name that includes it.
For example:
control c() {
  table t() { ... }  apply { t.apply() }
}
control d() {
  c() cinst;
  apply { cinst.apply(); }
}
After inlining we will get:
control d() {
  @name("cinst.t") table cinst_t() { ... }
  apply { cinst_t.apply(); }
}
So the externally visible name for the table is "cinst.t"
*/
class ComputeNewNames : public Inspector {
    cstring prefix;
    P4::ReferenceMap *refMap;
    SymRenameMap *renameMap;

 public:
    ComputeNewNames(cstring prefix, P4::ReferenceMap *refMap, SymRenameMap *renameMap)
        : prefix(prefix), refMap(refMap), renameMap(renameMap) {
        BUG_CHECK(!prefix.isNullOrEmpty(), "Null prefix");
        CHECK_NULL(refMap);
        CHECK_NULL(renameMap);
    }

    void rename(const IR::Declaration *decl) {
        BUG_CHECK(decl->is<IR::IAnnotated>(), "%1%: no annotations", decl);
        cstring name = decl->externalName();
        cstring extName;
        if (name.startsWith(".")) {
            // Do not change the external name of objects starting with a leading dot
            extName = name;
        } else {
            extName = prefix + "." + name;
        }
        // Replace non-alphanumeric characters with an underscore.
        std::string baseName = extName.c_str();
        std::replace_if(
            baseName.begin(), baseName.end(),
            [](char ch) { return (::isalnum(ch) == 0) && ch != '_'; }, '_');
        renameMap->setNewName(decl, refMap->newName(baseName), extName);
    }
    void postorder(const IR::P4Table *table) override { rename(table); }
    void postorder(const IR::P4ValueSet *set) override { rename(set); }
    void postorder(const IR::P4Action *action) override { rename(action); }
    void postorder(const IR::Declaration_Instance *instance) override { rename(instance); }
    void postorder(const IR::Declaration_Variable *decl) override { rename(decl); }
};

/**
Perform multiple substitutions and rename global objects, such as
tables, actions and instances.  Unfortunately these transformations
have to be performed at the same time, because otherwise the refMap
is invalidated.
*/
class Substitutions : public SubstituteParameters {
    P4::ReferenceMap *refMap;       // updated
    const SymRenameMap *renameMap;  // map with new names for global objects

 public:
    Substitutions(P4::ReferenceMap *refMap, P4::ParameterSubstitution *subst,
                  P4::TypeVariableSubstitution *tvs, const SymRenameMap *renameMap)
        : SubstituteParameters(refMap, subst, tvs), refMap(refMap), renameMap(renameMap) {
        CHECK_NULL(refMap);
        CHECK_NULL(renameMap);
    }
    const IR::Node *postorder(IR::P4Table *table) override {
        auto orig = getOriginal<IR::IDeclaration>();
        cstring newName = renameMap->getName(orig);
        cstring extName = renameMap->getExtName(orig);
        LOG3("Renaming " << dbp(orig) << " to " << newName << " (" << extName << ")");
        return new IR::P4Table(table->srcInfo, newName,
                               IR::Annotations::setNameAnnotation(table->annotations, extName),
                               table->properties);
    }
    const IR::Node *postorder(IR::P4ValueSet *set) override {
        auto orig = getOriginal<IR::IDeclaration>();
        cstring newName = renameMap->getName(orig);
        cstring extName = renameMap->getExtName(orig);
        LOG3("Renaming " << dbp(orig) << " to " << newName << "(" << extName << ")");
        return new IR::P4ValueSet(set->srcInfo, newName,
                                  IR::Annotations::setNameAnnotation(set->annotations, extName),
                                  set->elementType, set->size);
    }
    const IR::Node *postorder(IR::P4Action *action) override {
        auto orig = getOriginal<IR::IDeclaration>();
        cstring newName = renameMap->getName(orig);
        cstring extName = renameMap->getExtName(orig);
        LOG3("Renaming " << dbp(orig) << " to " << newName << "(" << extName << ")");
        return new IR::P4Action(action->srcInfo, newName,
                                IR::Annotations::setNameAnnotation(action->annotations, extName),
                                action->parameters, action->body);
    }
    const IR::Node *postorder(IR::Declaration_Instance *instance) override {
        auto orig = getOriginal<IR::IDeclaration>();
        cstring newName = renameMap->getName(orig);
        cstring extName = renameMap->getExtName(orig);
        LOG3("Renaming " << dbp(orig) << " to " << newName << "(" << extName << ")");
        instance->name = newName;
        IR::Annotations::addOrReplace(instance->annotations, IR::Annotation::nameAnnotation,
                                      new IR::StringLiteral(extName));
        return instance;
    }
    const IR::Node *postorder(IR::Declaration_Variable *decl) override {
        auto orig = getOriginal<IR::IDeclaration>();
        cstring newName = renameMap->getName(orig);
        cstring extName = renameMap->getExtName(orig);
        LOG3("Renaming " << dbp(orig) << " to " << newName << "(" << extName << ")");
        decl->name = newName;
        IR::Annotations::addOrReplace(decl->annotations, IR::Annotation::nameAnnotation,
                                      new IR::StringLiteral(extName));
        return decl;
    }
    const IR::Node *postorder(IR::PathExpression *expression) override {
        LOG3("(Substitutions) visiting" << dbp(getOriginal()));
        auto decl = refMap->getDeclaration(expression->path, true);
        auto param = decl->to<IR::Parameter>();
        if (param != nullptr && subst->contains(param)) {
            // This path is the same as in SubstituteParameters
            auto value = subst->lookup(param)->expression;
            LOG3("(Substitutions) Replaced " << dbp(expression) << " for parameter " << decl
                                             << " with " << dbp(value));
            return value;
        }

        cstring newName;
        if (renameMap->isRenamed(decl))
            newName = renameMap->getName(decl);
        else
            newName = expression->path->name;
        IR::ID newid(expression->path->srcInfo, newName, expression->path->name.originalName);
        auto newpath = new IR::Path(newid, expression->path->absolute);
        auto result = new IR::PathExpression(newpath);
        refMap->setDeclaration(newpath, decl);
        LOG3("(Substitutions) replaced " << dbp(getOriginal()) << " with " << dbp(result));
        return result;
    }
};
}  // namespace

template <class T>
const T *PerInstanceSubstitutions::rename(ReferenceMap *refMap, const IR::Node *node) {
    Substitutions rename(refMap, &paramSubst, &tvs, &renameMap);
    auto convert = node->apply(rename);
    CHECK_NULL(convert);
    auto result = convert->to<T>();
    CHECK_NULL(result);
    return result;
}

void InlineList::analyze() {
    P4::CallGraph<const IR::IContainer *> cg("Call-graph");

    for (auto m : inlineMap) {
        auto inl = m.second;
        if (inl->invocations.size() == 0) {
            LOG3("Callee " << inl->callee << " in " << inl->caller << " is never invoked");
            continue;
        }
        auto it = inl->invocations.begin();
        auto first = *it;
        if (!allowMultipleCalls && inl->invocations.size() > 1) {
            ++it;
            auto second = *it;
            ::P4::error(ErrorType::ERR_UNSUPPORTED_ON_TARGET,
                        "Multiple invocations of the same object "
                        "not supported on this target: %1%, %2%",
                        first, second);
            continue;
        }
        cg.calls(inl->caller, inl->callee);
    }

    // must inline from leaves up
    std::vector<const IR::IContainer *> order;
    cg.sort(order);
    for (auto c : order) {
        // This is quadratic, but hopefully the call graph is not too large
        for (auto m : inlineMap) {
            auto inl = m.second;
            if (inl->caller == c) toInline.push_back(inl);
        }
    }

    std::reverse(toInline.begin(), toInline.end());
}

InlineSummary *InlineList::next() {
    if (toInline.size() == 0) return nullptr;
    auto result = new InlineSummary();
    std::set<const IR::IContainer *> processing;
    while (!toInline.empty()) {
        auto toadd = toInline.back();
        if (processing.find(toadd->callee) != processing.end()) break;
        toInline.pop_back();
        result->add(toadd);
        processing.emplace(toadd->caller);
    }
    return result;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void DiscoverInlining::postorder(const IR::MethodCallStatement *statement) {
    LOG4("Visiting " << dbp(statement) << statement);
    auto mi = MethodInstance::resolve(statement, refMap, typeMap);
    if (!mi->isApply()) return;
    auto am = mi->to<P4::ApplyMethod>();
    CHECK_NULL(am);
    if (!am->applyObject->is<IR::Type_Control>() && !am->applyObject->is<IR::Type_Parser>()) return;
    auto instantiation = am->object->to<IR::Declaration_Instance>();
    if (instantiation != nullptr)
        inlineList->addInvocation(instantiation, statement);
    else
        BUG_CHECK(am->object->is<IR::Parameter>(), "%1% expected a constructor parameter",
                  am->object);
}

void DiscoverInlining::visit_all(const IR::Block *block) {
    for (auto it : block->constantValue) {
        if (it.second == nullptr) continue;
        if (it.second->is<IR::Block>()) {
            visit(it.second->getNode());
        }
        if (::P4::errorCount() > 0) return;
    }
}

bool DiscoverInlining::preorder(const IR::ControlBlock *block) {
    LOG4("Visiting " << block);
    if (getContext()->node->is<IR::ParserBlock>()) {
        ::P4::error(ErrorType::ERR_INVALID, "%1%: instantiation of control in parser", block->node);
        return false;
    } else if (getContext()->node->is<IR::ControlBlock>() && allowControls) {
        auto parent = getContext()->node->to<IR::ControlBlock>();
        LOG3("Will inline " << dbp(block) << "@" << dbp(block->node) << " into " << dbp(parent));
        auto instance = block->node->to<IR::Declaration_Instance>();
        auto callee = block->container;
        inlineList->addInstantiation(parent->container, callee, instance);
    }

    visit_all(block);
    if (::P4::errorCount() > 0) return false;
    visit(block->container->body);
    return false;
}

bool DiscoverInlining::preorder(const IR::ParserBlock *block) {
    LOG4("Visiting " << block);
    if (getContext()->node->is<IR::ControlBlock>()) {
        ::P4::error(ErrorType::ERR_INVALID, "%1%: instantiation of parser in control", block->node);
        return false;
    } else if (getContext()->node->is<IR::ParserBlock>()) {
        auto parent = getContext()->node->to<IR::ParserBlock>();
        LOG3("Will inline " << block << "@" << block->node << " into " << parent);
        auto instance = block->node->to<IR::Declaration_Instance>();
        auto callee = block->container;
        inlineList->addInstantiation(parent->container, callee, instance);
    }
    visit_all(block);
    if (::P4::errorCount() > 0) return false;
    visit(block->container->states, "states");
    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

Visitor::profile_t GeneralInliner::init_apply(const IR::Node *node) {
    ResolveReferences solver(refMap);
    TypeChecking typeChecker(refMap, typeMap);
    refMap->clear();
    node->apply(solver);
    (void)node->apply(typeChecker);
    return AbstractInliner::init_apply(node);
}

/* Build the substitutions needed for args and locals of the thing being inlined.
 * P4Block here may be either P4Control or P4Parser.
 * P4BlockType should be either Type_Control or Type_Parser to match the P4Block. */
template <class P4Block, class P4BlockType>
void GeneralInliner::inline_subst(P4Block *caller,
                                  IR::IndexedVector<IR::Declaration> P4Block::*blockLocals,
                                  const P4BlockType *P4Block::*blockType) {
    LOG3("Analyzing " << dbp(caller));
    IR::IndexedVector<IR::Declaration> locals;
    P4BlockType *type = (caller->*blockType)->clone();
    IR::Vector<IR::Annotation> annos = type->annotations;
    for (auto s : caller->*blockLocals) {
        /* Even if we inline the block, the declaration may still be needed.
           Consider this example:
           control D() { apply { } }
           control C()(D d) { apply { d.apply(); }}
           control I() {
               D() d;  // we can't delete this instantiation after inlining, since it's used below
               C(d) c;
               apply { c.apply(); }
           }
        */
        locals.push_back(s);
        const IR::Declaration_Instance *inst = s->template to<IR::Declaration_Instance>();
        if (inst == nullptr || workToDo->declToCallee.find(inst) == workToDo->declToCallee.end()) {
            // not a call
        } else {
            auto callee = workToDo->declToCallee[inst]->to<P4Block>();
            CHECK_NULL(callee);
            auto substs = new PerInstanceSubstitutions();
            workToDo->substitutions[inst] = substs;

            // Propagate annotations
            for (const auto *ann : (callee->*blockType)->annotations) {
                if (Inline::isAnnotationNoPropagate(ann->name)) continue;
                IR::Annotations::addIfNew(annos, ann);
            }

            // Substitute constructor parameters
            substs->paramSubst.populate(callee->getConstructorParameters(), inst->arguments);
            if (auto spec = inst->type->to<IR::Type_Specialized>()) {
                substs->tvs.setBindings(callee->getNode(), callee->getTypeParameters(),
                                        spec->arguments);
            }

            // Must rename callee local objects prefixing them with their instance name.
            cstring prefix = inst->externalName();
            ComputeNewNames cnn(prefix, refMap, &substs->renameMap);
            (void)callee->apply(cnn);  // populates substs.renameMap

            // Use temporaries for these parameters
            std::set<const IR::Parameter *> useTemporary;

            const IR::MethodCallStatement *call = nullptr;
            const IR::MethodCallStatement *firstCall = nullptr;  // to get directionless parameters
            for (auto m : workToDo->callToInstance) {
                if (m.second != inst) continue;
                if (call) {
                    if (!call->equiv(*m.first)) {
                        call = nullptr;
                        break;
                    }
                } else {
                    call = firstCall = m.first;
                }
            }
            CHECK_NULL(firstCall);
            MethodInstance *mi = MethodInstance::resolve(firstCall, refMap, typeMap);
            if (call != nullptr) {
                // All call sites are the same (call is one of them), so we use the
                // same arguments in all cases.  So we can avoid copies if args do
                // not alias
                std::map<const IR::Parameter *, const LocationSet *> locationSets;
                FindLocationSets fls(refMap, typeMap);

                for (auto param : *mi->substitution.getParametersInArgumentOrder()) {
                    auto arg = mi->substitution.lookup(param);
                    auto ls = fls.locations(arg->expression);
                    locationSets.emplace(param, ls);
                }

                for (auto param1 : *mi->substitution.getParametersInArgumentOrder()) {
                    auto ls1 = ::P4::get(locationSets, param1);
                    for (auto param2 : *mi->substitution.getParametersInArgumentOrder()) {
                        if (param1 == param2) continue;
                        auto ls2 = ::P4::get(locationSets, param2);
                        if (ls1->overlaps(ls2)) {
                            LOG4("Arg for " << dbp(param1) << " aliases with arg for "
                                            << dbp(param2) << ": using temp");
                            useTemporary.emplace(param1);
                            useTemporary.emplace(param2);
                        }
                    }
                }
            }

            // Substitute applyParameters
            // with fresh variable names or with the call arguments.
            for (auto param : callee->getApplyParameters()->parameters) {
                if (param->direction == IR::Direction::None) {
                    auto initializer = mi->substitution.lookup(param);
                    substs->paramSubst.add(param, initializer);
                    continue;
                }
                if (call != nullptr && (useTemporary.find(param) == useTemporary.end()) &&
                    mi->substitution.contains(param))
                // This may not be true for @optional parameters
                {
                    // Substitute argument directly
                    CHECK_NULL(mi);
                    auto initializer = mi->substitution.lookup(param);
                    LOG3("Substituting callee parameter " << dbp(param) << " with "
                                                          << dbp(initializer));
                    substs->paramSubst.add(param, initializer);
                } else {
                    // use a temporary variable
                    cstring newName = refMap->newName(param->name.name.string_view());
                    auto path = new IR::PathExpression(
                        param->srcInfo, new IR::Path(param->srcInfo, IR::ID(newName)));
                    substs->paramSubst.add(param, new IR::Argument(path));
                    LOG3("Replacing " << param->name << " with " << newName);
                    auto vardecl =
                        new IR::Declaration_Variable(newName, param->annotations, param->type);
                    locals.push_back(vardecl);
                }
            }

            /* We will perform these substitutions twice: once here, to
               compute the names for the locals that we need to inline here,
               and once again at the call site (where we do additional
               substitutions, including the callee parameters). */
            auto clone = substs->rename<P4Block>(refMap, callee);
            for (auto i : clone->*blockLocals) locals.push_back(i);
        }
    }
    caller->*blockLocals = locals;
    type->annotations = std::move(annos);
    caller->*blockType = type;
}

const IR::Node *GeneralInliner::preorder(IR::P4Control *caller) {
    // prepares the code to inline
    auto orig = getOriginal<IR::P4Control>();
    if (toInline->callerToWork.find(orig) == toInline->callerToWork.end()) {
        prune();
        return caller;
    }

    workToDo = &toInline->callerToWork[orig];
    inline_subst(caller, &IR::P4Control::controlLocals, &IR::P4Control::type);
    visit(caller->body);
    list->replace(orig, caller);
    workToDo = nullptr;
    prune();
    return caller;
}

const IR::Node *GeneralInliner::preorder(IR::MethodCallStatement *statement) {
    if (workToDo == nullptr) return statement;
    auto orig = getOriginal<IR::MethodCallStatement>();
    if (workToDo->callToInstance.find(orig) == workToDo->callToInstance.end()) return statement;
    LOG3("Inlining invocation " << dbp(orig));
    auto decl = workToDo->callToInstance[orig];
    CHECK_NULL(decl);

    auto called = workToDo->declToCallee[decl];
    if (!called->is<IR::P4Control>())
        // Parsers are inlined in the ParserState processor
        return statement;

    auto callee = called->to<IR::P4Control>();
    IR::IndexedVector<IR::StatOrDecl> body;
    // clone the substitution: it may be reused for multiple invocations
    auto substs = new PerInstanceSubstitutions(*workToDo->substitutions[decl]);

    auto mi = MethodInstance::resolve(statement->methodCall, refMap, typeMap);
    for (auto param : *mi->substitution.getParametersInArgumentOrder()) {
        LOG3("Looking for " << param->name);
        auto initializer = substs->paramSubst.lookup(param);
        auto arg = mi->substitution.lookup(param);
        if ((param->direction == IR::Direction::In || param->direction == IR::Direction::InOut)) {
            if (!initializer->expression->equiv(*arg->expression)) {
                auto stat = new IR::AssignmentStatement(initializer->expression, arg->expression);
                body.push_back(stat);
            }
        } else if (param->direction == IR::Direction::Out) {
            auto paramType = typeMap->getType(param, true);
            // This is important, since this variable may be used many times.
            DoResetHeaders::generateResets(typeMap, paramType, initializer->expression, &body);
        } else if (param->direction == IR::Direction::None) {
            // already set; the value must be the same, or else we cannot compile
            auto initializer = mi->substitution.lookup(param);
            auto prev = substs->paramSubst.lookup(param);
            if (!initializer->equiv(*prev))
                // This is a compile-time constant, since this is a non-directional
                // parameter, so the value should be independent on the context.
                ::P4::error(ErrorType::ERR_INVALID,
                            "%1%: non-directional parameters must be substitued with the "
                            "same value in all invocations; two different substitutions are "
                            "%2% and %3%",
                            param, initializer, prev);
            continue;
        }
    }

    // inline actual body
    callee = substs->rename<IR::P4Control>(refMap, callee);
    body.append(callee->body->components);

    // Copy values of out and inout parameters
    for (auto param : *mi->substitution.getParametersInArgumentOrder()) {
        if (param->direction == IR::Direction::InOut || param->direction == IR::Direction::Out) {
            auto left = mi->substitution.lookup(param);
            auto arg = substs->paramSubst.lookupByName(param->name);
            if (!left->expression->equiv(*arg->expression)) {
                auto copyout =
                    new IR::AssignmentStatement(left->expression, arg->expression->clone());
                body.push_back(copyout);
            }
        }
    }

    auto annotations = callee->type->annotations.where(
        [](const IR::Annotation *a) { return a->name != IR::Annotation::nameAnnotation; });
    auto result = new IR::BlockStatement(statement->srcInfo, annotations, body);
    LOG3("Replacing " << dbp(orig) << " with " << dbp(result));
    prune();
    return result;
}

namespace {
class ComputeNewStateNames : public Inspector {
    ReferenceMap *refMap;
    cstring prefix;
    cstring acceptName;
    std::map<cstring, cstring> *stateRenameMap;

 public:
    ComputeNewStateNames(ReferenceMap *refMap, cstring prefix, cstring acceptName,
                         std::map<cstring, cstring> *stateRenameMap)
        : refMap(refMap), prefix(prefix), acceptName(acceptName), stateRenameMap(stateRenameMap) {
        CHECK_NULL(refMap);
        CHECK_NULL(stateRenameMap);
    }
    bool preorder(const IR::ParserState *state) override {
        cstring newName;
        if (state->name.name == IR::ParserState::reject) {
            newName = state->name.name;
        } else if (state->name.name == IR::ParserState::accept) {
            newName = acceptName;
        } else {
            std::string base = prefix + "_" + state->name.name;
            newName = refMap->newName(base);
        }
        stateRenameMap->emplace(state->name.name, newName);
        return false;  // prune
    }
};

/**
Renames the states in a parser for inlining.  We cannot rely on the
ReferenceMap for identifying states - it is currently inconsistent,
so we rely on the fact that state names only appear in very
specific places.
*/
class RenameStates : public Transform {
    std::map<cstring, cstring> *stateRenameMap;

 public:
    explicit RenameStates(std::map<cstring, cstring> *stateRenameMap)
        : stateRenameMap(stateRenameMap) {
        CHECK_NULL(stateRenameMap);
    }
    const IR::Node *preorder(IR::Path *path) override {
        // This is certainly a state name, by the way we organized the visitors
        cstring newName = ::P4::get(stateRenameMap, path->name);
        path->name = IR::ID(path->name.srcInfo, newName, path->name.originalName);
        return path;
    }
    const IR::Node *preorder(IR::SelectExpression *expression) override {
        parallel_visit(expression->selectCases, "selectCases", 1);
        prune();
        return expression;
    }
    const IR::Node *preorder(IR::SelectCase *selCase) override {
        visit(selCase->state);
        prune();
        return selCase;
    }
    const IR::Node *preorder(IR::ParserState *state) override {
        if (state->name.name == IR::ParserState::accept ||
            state->name.name == IR::ParserState::reject) {
            prune();
            return state;
        }
        cstring newName = ::P4::get(stateRenameMap, state->name.name);
        state->name.name = newName;
        if (state->selectExpression != nullptr) visit(state->selectExpression);
        prune();
        return state;
    }
    const IR::Node *preorder(IR::P4Parser *parser) override {
        visit(parser->states, "states");
        prune();
        return parser;
    }
};
}  // namespace

const IR::Node *GeneralInliner::preorder(IR::ParserState *state) {
    LOG3("Visiting state " << dbp(state));
    auto states = new IR::IndexedVector<IR::ParserState>();
    IR::IndexedVector<IR::StatOrDecl> current;

    // Scan the statements to find a parser call instruction
    auto srcInfo = state->srcInfo;
    auto annotations = state->annotations;
    IR::ID name = state->name;
    for (auto e = state->components.begin(); e != state->components.end(); ++e) {
        if (!(*e)->is<IR::MethodCallStatement>()) {
            current.push_back(*e);
            continue;
        }
        auto call = (*e)->to<IR::MethodCallStatement>();
        if (workToDo->callToInstance.find(call) == workToDo->callToInstance.end()) {
            current.push_back(*e);
            continue;
        }

        LOG3("Inlining invocation " << dbp(call));
        auto decl = workToDo->callToInstance[call];
        CHECK_NULL(decl);

        auto called = workToDo->declToCallee[decl];
        auto callee = called->to<IR::P4Parser>();
        // clone the substitution: it may be reused for multiple invocations
        auto substs = new PerInstanceSubstitutions(*workToDo->substitutions[decl]);

        auto mi = MethodInstance::resolve(call->methodCall, refMap, typeMap);
        // Evaluate in and inout parameters in order.
        for (auto param : callee->getApplyParameters()->parameters) {
            auto initializer = mi->substitution.lookup(param);
            LOG3("Looking for " << param->name);
            if (param->direction == IR::Direction::In || param->direction == IR::Direction::InOut) {
                auto arg = substs->paramSubst.lookupByName(param->name);
                if (!arg->expression->equiv(*initializer->expression)) {
                    auto stat =
                        new IR::AssignmentStatement(arg->expression, initializer->expression);
                    current.push_back(stat);
                }
            } else if (param->direction == IR::Direction::Out) {
                auto arg = substs->paramSubst.lookupByName(param->name);
                auto paramType = typeMap->getType(param, true);
                // This is important, since this variable may be used many times.
                DoResetHeaders::generateResets(typeMap, paramType, arg->expression, &current);
            } else if (param->direction == IR::Direction::None) {
                // already set; the value must be the same, or else we cannot compile
                auto prev = substs->paramSubst.lookup(param);
                CHECK_NULL(prev);
                if (!initializer->equiv(*prev))
                    // This is a compile-time constant, since this is a non-directional
                    // parameter, so the value should be independent on the context.
                    ::P4::error(ErrorType::ERR_INVALID,
                                "%1%: non-directional parameters must be substitued with the "
                                "same value in all invocations; two different substitutions are "
                                "%2% and %3%",
                                param, initializer, prev);
                continue;
            }
        }

        /**
         * Check if there are already inlined states of the callee subparser of the same instance
         * as this one with the same arguments, no statements after this call and transition
         * to the same state (without select expression).
         * If yes, we can reuse those states as after returning from callee subparser the parser
         * continues in the same path.
         */
        if (optimizeParserInlining && (e + 1) == state->components.end() &&
            state->selectExpression->is<IR::PathExpression>()) {
            auto invoc = std::make_pair(call, state->selectExpression->to<IR::PathExpression>());
            if (workToDo->invocationToState.find(invoc) != workToDo->invocationToState.end()) {
                IR::ID reusedStartStateName = workToDo->invocationToState[invoc];
                auto newState = new IR::ParserState(srcInfo, name, annotations, current,
                                                    new IR::PathExpression(reusedStartStateName));
                states->push_back(newState);
                LOG3("Reusing inlined state: "
                     << reusedStartStateName << " in new state: " << dbp(newState) << std::endl
                     << "Replacing " << dbp(state) << " with " << states->size() << " states");
                prune();
                return states;
            }
        }

        callee = substs->rename<IR::P4Parser>(refMap, callee);

        cstring nextState = refMap->newName(state->name.name.string_view());
        std::map<cstring, cstring> renameMap;
        ComputeNewStateNames cnn(refMap, callee->name.name, nextState, &renameMap);
        cnn.setCalledBy(this);
        (void)callee->apply(cnn);
        RenameStates rs(&renameMap);
        rs.setCalledBy(this);
        auto renamed = callee->apply(rs);
        IR::ID newStartName(::P4::get(renameMap, IR::ParserState::start), IR::ParserState::start);
        auto newState = new IR::ParserState(srcInfo, name, annotations, current,
                                            new IR::PathExpression(newStartName));
        states->push_back(newState);
        for (auto s : renamed->to<IR::P4Parser>()->states) {
            if (s->name == IR::ParserState::accept || s->name == IR::ParserState::reject) continue;
            states->push_back(s);
        }

        /**
         * If there is no other statement after this invocation of the subparser and
         * transition does not use select expression, we store the ID of the inlined subparser's
         * start state, currently processed invocation statement and the transition statement
         * expression.
         */
        if (optimizeParserInlining && (e + 1) == state->components.end() &&
            state->selectExpression->is<IR::PathExpression>()) {
            auto invoc = std::make_pair(call, state->selectExpression->to<IR::PathExpression>());
            auto ret = workToDo->invocationToState.emplace(invoc, newStartName);
            LOG3("Saving new start state ID: " << newStartName << " for call: " << dbp(call) << " ("
                                               << call << ") and transition: "
                                               << dbp(state->selectExpression));
            BUG_CHECK(ret.second == true || newStartName == ret.first->second,
                      "State: %1% already saved, can not save: %2%!", ret.first->second,
                      newStartName);
        }

        // Prepare next state
        name = IR::ID(nextState, state->name);
        annotations.clear();
        current.clear();

        // Copy back out and inout parameters
        for (auto param : callee->getApplyParameters()->parameters) {
            auto left = mi->substitution.lookup(param)->expression;
            if (param->direction == IR::Direction::InOut ||
                param->direction == IR::Direction::Out) {
                auto arg = substs->paramSubst.lookupByName(param->name);
                if (!left->equiv(*arg->expression)) {
                    auto copyout = new IR::AssignmentStatement(left, arg->expression->clone());
                    current.push_back(copyout);
                }
            }
        }
    }

    if (!states->empty()) {
        // Create final state
        auto newState =
            new IR::ParserState(srcInfo, name, annotations, current, state->selectExpression);
        states->push_back(newState);
        LOG3("Replacing with " << states->size() << " states");
        prune();
        return states;
    }
    prune();
    return state;
}

const IR::Node *GeneralInliner::preorder(IR::P4Parser *caller) {
    // prepares the code to inline
    auto orig = getOriginal<IR::P4Parser>();
    if (toInline->callerToWork.find(orig) == toInline->callerToWork.end()) {
        prune();
        return caller;
    }

    workToDo = &toInline->callerToWork[orig];
    inline_subst(caller, &IR::P4Parser::parserLocals, &IR::P4Parser::type);
    visit(caller->states, "states");
    list->replace(orig, caller);
    workToDo = nullptr;
    prune();
    return caller;
}

// set of annotations to _not_ propagate during inlining
std::set<cstring> Inline::noPropagateAnnotations = {"name"_cs};

}  // namespace P4
