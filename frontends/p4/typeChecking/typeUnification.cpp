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

#include "typeUnification.h"

#include "frontends/p4/typeChecking/typeChecker.h"
#include "frontends/p4/typeMap.h"
#include "typeConstraints.h"

namespace P4 {

using namespace literals;

/// Unifies a call with a prototype.
bool TypeUnification::unifyCall(const BinaryConstraint *constraint) {
    // These are canonical types.
    auto dest = constraint->left->to<IR::Type_MethodBase>();
    auto src = constraint->right->to<IR::Type_MethodCall>();
    CHECK_NULL(dest);
    CHECK_NULL(src);
    LOG3("Unifying function " << dest << " with caller " << src);

    for (auto tv : dest->typeParameters->parameters) constraints->addUnifiableTypeVariable(tv);
    if (dest->returnType == nullptr) {
        auto c = constraint->create(IR::Type_Void::get(), src->returnType);
        c->setError("Return type is '%1%' instead of 'void'", {src->returnType});
        constraints->add(c);
    } else {
        auto c = constraint->create(dest->returnType, src->returnType);
        c->setError("Return type '%1%' cannot be used for '%2%'",
                    {dest->returnType, src->returnType});
        constraints->add(c);
    }
    constraints->addUnifiableTypeVariable(src->returnType);  // always a type variable

    // Adding type parameters last will ensure that they are matched first
    // since the algorithms pops constraints from the back of the list
    for (auto tv : dest->typeParameters->parameters) constraints->addUnifiableTypeVariable(tv);

    if (dest->parameters->size() < src->arguments->size())
        return constraint->reportError(constraints->getCurrentSubstitution(),
                                       "%1%: %2% arguments supplied while %3% are expected", dest,
                                       src->arguments->size(), dest->parameters->size());

    auto paramIt = dest->parameters->begin();
    // keep track of parameters that have not been matched yet
    absl::flat_hash_map<cstring, const IR::Parameter *, Util::Hash> left;
    for (auto p : dest->parameters->parameters) left.emplace(p->name, p);

    for (auto arg : *src->arguments) {
        cstring argName = arg->argument->name.name;
        bool named = !argName.isNullOrEmpty();
        const IR::Parameter *param;

        if (named) {
            param = dest->parameters->getParameter(argName);
            if (param == nullptr)
                return constraint->reportError(constraints->getCurrentSubstitution(),
                                               "No parameter named '%1%'", arg->argument->name);
        } else {
            if (paramIt == dest->parameters->end())
                return constraint->reportError(constraints->getCurrentSubstitution(),
                                               "Too many arguments for call");
            param = *paramIt;
        }

        if (!left.erase(param->name)) {
            // This shold have been checked by the CheckNamedArgs pass.
            BUG("%1%: Duplicate argument name?", param->name);
        }

        if (arg->type->is<IR::Type_Dontcare>() && param->direction != IR::Direction::Out)
            return constraint->reportError(
                constraints->getCurrentSubstitution(),
                "%1%: don't care argument is only allowed for out parameters", arg->srcInfo);
        if ((param->direction == IR::Direction::Out || param->direction == IR::Direction::InOut) &&
            (!arg->leftValue))
            return constraint->reportError(
                constraints->getCurrentSubstitution(),
                "%1%: Read-only value used for out/inout parameter '%2%'", arg->srcInfo, param);
        else if (param->direction == IR::Direction::None && !arg->compileTimeConstant)
            return constraint->reportError(constraints->getCurrentSubstitution(),
                                           "%1%: argument used for directionless parameter '%2%' "
                                           "must be a compile-time constant",
                                           arg->argument, param);

        if (param->direction != IR::Direction::None && param->type->is<IR::Type_Extern>()) {
            if (optarg) continue;
            return constraint->reportError(constraints->getCurrentSubstitution(),
                                           "%1%: extern values cannot be passed in/out/inout",
                                           param);
        }

        P4::BinaryConstraint *c;
        if (!param->hasOut())
            c = new CanBeImplicitlyCastConstraint(param->type, arg->type, constraint);
        else
            c = constraint->create(param->type, arg->type);
        c->setError("Type of argument '%1%' (%2%) does not match type of parameter '%3%' (%4%)",
                    {arg, arg->type, param, param->type});
        constraints->add(c);
        if (!named) ++paramIt;
    }

    // Check remaining parameters: they must be all optional or have default values
    for (auto p : left) {
        bool opt = p.second->isOptional() || p.second->defaultValue != nullptr;
        if (opt) continue;
        return constraint->reportError(constraints->getCurrentSubstitution(),
                                       "%1%: No argument supplied for parameter", p.second);
    }

    if (src->typeArguments->size() != 0) {
        if (dest->typeParameters->size() != src->typeArguments->size())
            return constraint->reportError(
                constraints->getCurrentSubstitution(),
                "%1%: %2% type parameters expected, but %3% type arguments supplied", dest,
                dest->typeParameters->size(), src->typeArguments->size());

        size_t i = 0;
        for (auto tv : dest->typeParameters->parameters) {
            auto type = src->typeArguments->at(i++);
            // variable type represents type of formal method argument
            // written beetween angle brackets, and tv should be replaced
            // with type of an actual argument
            auto c = constraint->create(type, tv);
            c->setError("Type parameter '%1%' substituted with type argument '%2%'", {type, tv});
            constraints->add(c);
        }
    }

    return true;
}

// skipReturnValues is needed because the return type of a package
// is the package itself, so checking it gets into an infinite loop.
bool TypeUnification::unifyFunctions(const BinaryConstraint *constraint, bool skipReturnValues) {
    auto dest = constraint->left->to<IR::Type_MethodBase>();
    auto src = constraint->right->to<IR::Type_MethodBase>();
    CHECK_NULL(dest);
    CHECK_NULL(src);
    LOG3("Unifying functions " << dest << " with " << src);

    for (auto tv : dest->typeParameters->parameters) constraints->addUnifiableTypeVariable(tv);
    for (auto tv : src->typeParameters->parameters) constraints->addUnifiableTypeVariable(tv);

    if ((src->returnType == nullptr) != (dest->returnType == nullptr))
        return constraint->reportError(
            constraints->getCurrentSubstitution(),
            "Cannot unify functions with different return types '%1%' and '%2%'", dest, src);
    if (!skipReturnValues && src->returnType != nullptr)
        constraints->add(constraint->create(dest->returnType, src->returnType));

    auto sit = src->parameters->parameters.begin();
    for (auto dit : *dest->parameters->getEnumerator()) {
        if (sit == src->parameters->parameters.end()) {
            if (dit->isOptional()) continue;
            if (dit->defaultValue != nullptr) continue;
            return constraint->reportError(
                constraints->getCurrentSubstitution(),
                "Cannot unify functions with different number of arguments: %1% to %2%", src, dest);
        }
        if ((*sit)->direction != dit->direction)
            return constraint->reportError(
                constraints->getCurrentSubstitution(),
                "Cannot unify '%1%' parameter '%2%' with '%3%' parameter '%4%' "
                "because they have different directions",
                IR::directionToString((*sit)->direction), *sit,
                IR::directionToString(dit->direction), dit);
        constraints->add(constraint->create(dit->type, (*sit)->type));
        ++sit;
    }
    while (sit != src->parameters->parameters.end()) {
        if ((*sit)->isOptional()) {
            ++sit;
            continue;
        }
        if ((*sit)->defaultValue != nullptr) {
            ++sit;
            continue;
        }
        return constraint->reportError(constraints->getCurrentSubstitution(),
                                       "Cannot unify functions with different number of arguments: "
                                       "%1% to %2%",
                                       src, dest);
    }
    return true;
}

bool TypeUnification::unifyBlocks(const BinaryConstraint *constraint) {
    // These are canonical types.
    auto dest = constraint->left->to<IR::Type_ArchBlock>();
    auto src = constraint->right->to<IR::Type_ArchBlock>();
    CHECK_NULL(dest);
    CHECK_NULL(src);
    LOG3("Unifying blocks " << dest << " with " << src);
    if (dest->typeId() != src->typeId())
        return constraint->reportError(constraints->getCurrentSubstitution());
    for (auto tv : dest->typeParameters->parameters) constraints->addUnifiableTypeVariable(tv);
    for (auto tv : src->typeParameters->parameters) constraints->addUnifiableTypeVariable(tv);
    if (dest->is<IR::Type_Package>()) {
        // Two packages unify if and only if they have the same name
        // and if their corresponding parameters unify
        auto destPackage = dest->to<IR::Type_Package>();
        auto srcPackage = src->to<IR::Type_Package>();
        if (destPackage->name != srcPackage->name)
            return constraint->reportError(constraints->getCurrentSubstitution());
        auto destConstructor = dest->to<IR::Type_Package>()->getConstructorMethodType();
        auto srcConstructor = src->to<IR::Type_Package>()->getConstructorMethodType();
        return unifyFunctions(constraint->create(destConstructor, srcConstructor), true);
    } else if (dest->is<IR::IApply>()) {
        // parsers, controls
        auto srcapply = src->to<IR::IApply>()->getApplyMethodType();
        auto destapply = dest->to<IR::IApply>()->getApplyMethodType();
        return unifyFunctions(constraint->create(destapply, srcapply));
    }
    return constraint->reportError(constraints->getCurrentSubstitution());
}

bool TypeUnification::containsDots(const IR::Type_StructLike *type) {
    if (type->fields.empty()) return false;
    size_t lastIndex = type->fields.size() - 1;
    return type->fields.at(lastIndex)->type->is<IR::Type_Any>();
}

bool TypeUnification::containsDots(const IR::Type_BaseList *type) {
    if (type->components.empty()) return false;
    size_t lastIndex = type->components.size() - 1;
    return type->components.at(lastIndex)->is<IR::Type_Any>();
}

bool TypeUnification::unify(const BinaryConstraint *constraint) {
    auto dest = constraint->left;
    auto src = constraint->right;
    // These are canonical types.
    CHECK_NULL(dest);
    CHECK_NULL(src);
    LOG3("Unifying " << dest << " with " << src);

    if (src->is<IR::ITypeVar>()) {
        src = src->apply(constraints->replaceVariables)->to<IR::Type>();
    }
    if (dest->is<IR::ITypeVar>()) {
        dest = dest->apply(constraints->replaceVariables)->to<IR::Type>();
    }

    if (auto dsc = dest->to<IR::Type_SpecializedCanonical>()) {
        if (auto ssc = src->to<IR::Type_SpecializedCanonical>()) {
            if (dsc->arguments->size() != ssc->arguments->size())
                return constraint->reportError(
                    constraints->getCurrentSubstitution(),
                    "Type argument lists %1% and %2% have different lengths", dsc->arguments,
                    ssc->arguments);
            constraints->add(constraint->create(dsc->baseType, ssc->baseType));
            for (size_t i = 0; i < dsc->arguments->size(); i++) {
                constraints->add(constraint->create(dsc->arguments->at(i), ssc->arguments->at(i)));
            }
            return true;
        }
        constraints->add(constraint->create(dsc->substituted, src));
        return true;
    }
    // symmetric case
    if (auto ssc = src->to<IR::Type_SpecializedCanonical>()) {
        constraints->add(constraint->create(dest, ssc->substituted));
        return true;
    }

    if (typeMap->equivalent(dest, src)) return true;

    if (src->is<IR::Type_Dontcare>() || dest->is<IR::Type_Dontcare>()) return true;

    if (src->is<IR::Type_Any>()) {
        constraints->addUnifiableTypeVariable(src->to<IR::Type_Any>());
        constraints->add(constraint->create(dest, src));
        return true;
    } else if (dest->is<IR::Type_Any>()) {
        constraints->addUnifiableTypeVariable(dest->to<IR::Type_Any>());
        constraints->add(constraint->create(dest, src));
        return true;
    } else if (dest->is<IR::Type_ArchBlock>()) {
        // This case handles the comparison of Type_Parser with P4Parser
        // (and similarly for controls).
        if (auto cont = src->to<IR::IContainer>()) {
            if (cont->getType() != src) {
                // For Type_Package cont->getType() == const == src, causing an infinite loop
                constraints->add(constraint->create(dest, cont->getType()));
                return true;
            }
        }
        if (!src->is<IR::Type_ArchBlock>())
            return constraint->reportError(constraints->getCurrentSubstitution());
        return unifyBlocks(constraint->create(dest, src));
    } else if (dest->is<IR::Type_MethodBase>()) {
        if (src->is<IR::Type_MethodCall>()) return unifyCall(constraint);
        if (src->is<IR::Type_MethodBase>()) return unifyFunctions(constraint);
        return constraint->reportError(
            constraints->getCurrentSubstitution(),
            "Cannot unify non-function type '%1%' to function type '%2%'", src, dest);
    } else if (auto td = dest->to<IR::Type_BaseList>()) {
        if (!src->is<IR::Type_BaseList>())
            return constraint->reportError(constraints->getCurrentSubstitution());
        auto ts = src->to<IR::Type_BaseList>();
        bool hasDots = containsDots(ts);
        if (td->components.size() != ts->components.size() && !hasDots)
            return constraint->reportError(constraints->getCurrentSubstitution(),
                                           "Tuples with different sizes %1% vs %2%",
                                           td->components.size(), ts->components.size());
        IR::Vector<IR::Type> missingFields;
        size_t sourceSize = ts->components.size();
        for (size_t i = 0; i < td->components.size(); i++) {
            auto di = td->components.at(i);
            if (hasDots && (i >= sourceSize - 1)) {
                missingFields.push_back(di);
            } else {
                // Last field is the ... field
                auto si = ts->components.at(i);
                constraints->add(constraint->create(di, si));
            }
        }
        if (hasDots) {
            auto dotsType = new IR::Type_List(std::move(missingFields));
            auto dotsField = ts->components.at(sourceSize - 1);
            auto partial = new IR::Type_Fragment(dotsType);
            constraints->add(new EqualityConstraint(dotsField, partial, constraint));
        }
        return true;
    } else if (dest->is<IR::Type_Struct>() || dest->is<IR::Type_Header>()) {
        auto strct = dest->to<IR::Type_StructLike>();
        if (auto tpl = src->to<IR::Type_List>()) {
            bool hasDots = containsDots(tpl);
            size_t listSize = tpl->components.size();
            if (strct->fields.size() != listSize && !hasDots)
                return constraint->reportError(
                    constraints->getCurrentSubstitution(),
                    "Number of fields %1% in initializer %2% is different "
                    "than number of fields %3% in '%4%'",
                    tpl->components.size(), tpl, strct->fields.size(), strct);
            size_t index = 0;
            IR::IndexedVector<IR::StructField> missingFields;
            for (const IR::StructField *f : strct->fields) {
                if (!hasDots || listSize > index + 1) {
                    // Last field is the ... field
                    const IR::Type *tplField = tpl->components.at(index);
                    const IR::Type *destt = f->type;
                    constraints->add(constraint->create(destt, tplField));
                } else {
                    missingFields.push_back(f);
                }
                index++;
            }
            if (hasDots) {
                auto dotsType = new IR::Type_UnknownStruct(strct->name, std::move(missingFields));
                auto dotsField = tpl->components.at(listSize - 1);
                auto partial = new IR::Type_Fragment(dotsType);
                constraints->add(new EqualityConstraint(dotsField, partial, constraint));
            }
            return true;
        } else if (auto st = src->to<IR::Type_StructLike>()) {
            if (typeMap->strictStruct && strct->name != st->name &&
                !st->is<IR::Type_UnknownStruct>() && !strct->is<IR::Type_UnknownStruct>())
                return constraint->reportError(constraints->getCurrentSubstitution(),
                                               "Cannot unify '%1%' with '%2%'", st->name,
                                               strct->name);
            bool strctHasDots = containsDots(strct);
            bool stHasDots = containsDots(st);
            if (strctHasDots && stHasDots)
                return constraint->reportError(constraints->getCurrentSubstitution(),
                                               "Cannot unify %1% and %2% both of which "
                                               "contain unspecified fields",
                                               strct, st);
            // There is another case, in which each field of the source is unifiable with the
            // corresponding field of the destination, e.g., a struct containing tuples.
            if ((strct->fields.size() != st->fields.size()) && !strctHasDots && !stHasDots)
                return constraint->reportError(constraints->getCurrentSubstitution(),
                                               "Number of fields %1% in initializer different "
                                               "than number of fields in structure %2%: %3% to %4%",
                                               st->fields.size(), strct->fields.size(), st, strct);

            IR::IndexedVector<IR::StructField> missingFields;
            for (const IR::StructField *f : strct->fields) {
                auto stField = st->getField(f->name);
                if (stField == nullptr) {
                    if (stHasDots) {
                        missingFields.push_back(f);
                        continue;
                    } else {
                        return constraint->reportError(constraints->getCurrentSubstitution(),
                                                       "No initializer for field %1%", f);
                    }
                }
                auto c = constraint->create(f->type, stField->type);
                c->setError(
                    "Type of initializer '%1%' does not match type '%2%' of field '%3%' in '%4%'",
                    {stField->type, f->type, f, strct});
                constraints->add(c);
            }
            if (stHasDots) {
                auto dotsType = new IR::Type_UnknownStruct(st->name, std::move(missingFields));
                auto dotsField = st->getField("..."_cs);
                CHECK_NULL(dotsField);
                auto partial = new IR::Type_Fragment(dotsType);
                constraints->add(new EqualityConstraint(dotsField->type, partial, constraint));
            }
            return true;
        }

        return constraint->reportError(constraints->getCurrentSubstitution());
    } else if (dest->is<IR::Type_Base>()) {
        if (dest->is<IR::Type_Bits>() && src->is<IR::Type_InfInt>()) {
            constraints->addUnifiableTypeVariable(src->to<IR::Type_InfInt>());
            constraints->add(constraint->create(dest, src));
            return true;
        }
        if (src->is<IR::Type_Any>()) {
            constraints->addUnifiableTypeVariable(src->to<IR::Type_Any>());
            constraints->add(constraint->create(dest, src));
            return true;
        }
        if (auto senum = src->to<IR::Type_SerEnum>()) {
            if (constraint->is<P4::CanBeImplicitlyCastConstraint>()) {
                if (dest->is<IR::Type_Bits>()) {
                    // unify with enum's underlying type
                    auto stype = typeMap->getTypeType(senum->type, true);
                    return unify(constraint->create(stype, dest));
                }
            }
        }
        if (!src->is<IR::Type_Base>())
            return constraint->reportError(constraints->getCurrentSubstitution());

        bool success = (*src) == (*dest);
        if (!success) return constraint->reportError(constraints->getCurrentSubstitution());
        return true;
    } else if (dest->is<IR::Type_Declaration>() && src->is<IR::Type_Declaration>()) {
        if (dest->to<IR::Type_Declaration>()->name != src->to<IR::Type_Declaration>()->name)
            return constraint->reportError(constraints->getCurrentSubstitution());
        return true;
    } else if (auto dstack = dest->to<IR::Type_Array>()) {
        if (auto sstack = src->to<IR::Type_Array>()) {
            if (dstack->getSize() != sstack->getSize())
                return constraint->reportError(
                    constraints->getCurrentSubstitution(),
                    "cannot unify header stack '%1%' and '%2%' since they have different sizes",
                    dstack, sstack);
            constraints->add(constraint->create(dstack->elementType, sstack->elementType));
            return true;
        } else if (auto list = src->to<IR::Type_List>()) {
            bool hasDots = containsDots(list);
            size_t listSize = list->components.size();
            if (dstack->getSize() != listSize && !hasDots)
                return constraint->reportError(
                    constraints->getCurrentSubstitution(),
                    "Number of components %1% in initializer %2% is different "
                    "than stack size %3%",
                    listSize, list, dstack->getSize());
            for (size_t index = 0; index < listSize - 1; index++) {
                const IR::Type *comp = list->components.at(index);
                constraints->add(constraint->create(dstack->elementType, comp));
            }
            if (hasDots) {
                auto dotsType = new IR::Type_Array(
                    dstack->elementType,
                    new IR::Constant((unsigned)(dstack->getSize() - listSize + 1)));
                auto dotsField = list->components.at(listSize - 1);
                auto partial = new IR::Type_Fragment(dotsType);
                constraints->add(new EqualityConstraint(dotsField, partial, constraint));
            }
            return true;
        }
    } else if (dest->is<IR::Type_P4List>() && src->is<IR::Type_P4List>()) {
        auto dvec = dest->to<IR::Type_P4List>();
        auto svec = src->to<IR::Type_P4List>();
        constraints->add(constraint->create(dvec->elementType, svec->elementType));
        return true;
    }

    return constraint->reportError(constraints->getCurrentSubstitution());
}

}  // namespace P4
