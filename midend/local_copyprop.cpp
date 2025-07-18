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

#include "local_copyprop.h"

#include <unordered_set>

#include "expr_uses.h"
#include "frontends/common/copySrcInfo.h"
#include "frontends/p4/methodInstance.h"
#include "has_side_effects.h"
#include "ir/ir-generated.h"

namespace P4 {

using namespace literals;

/* helper function to get the 'outermost' containing expression in an lvalue */
static const IR::Expression *lvalue_out(const IR::Expression *exp) {
    if (auto ai = exp->to<IR::ArrayIndex>()) return lvalue_out(ai->left);
    if (auto hsr = exp->to<IR::HeaderStackItemRef>()) return lvalue_out(hsr->base());
    if (auto sl = exp->to<IR::AbstractSlice>()) return lvalue_out(sl->e0);
    if (auto mem = exp->to<IR::Member>()) return lvalue_out(mem->expr);
    return exp;
}

/* helper function to extract the side effects from an expression as a statement */
static const IR::Statement *makeSideEffectStatement(const IR::Expression *exp) {
    while (auto unary = exp->to<IR::Operation_Unary>()) exp = unary->expr;
    if (auto mc = exp->to<IR::MethodCallExpression>()) return new IR::MethodCallStatement(mc);
    BUG("Unhandled expression in makeSideEffectStatement");
    return nullptr;
}

/// convert an expression into a string that uniqely identifies the value referenced
/// return null cstring if not a reference to a constant thing.
static cstring expr_name(const IR::Expression *exp) {
    if (auto p = exp->to<IR::PathExpression>()) return p->path->name;
    if (auto m = exp->to<IR::Member>()) {
        if (auto base = expr_name(m->expr)) return base + "." + m->member;
    } else if (auto a = exp->to<IR::ArrayIndex>()) {
        if (auto k = a->right->to<IR::Constant>())
            if (auto base = expr_name(a->left)) return base + "." + std::to_string(k->asInt());
    }
    return cstring();
}

int DoLocalCopyPropagation::uid_ctr = 0;

/* LocalCopyPropagation does copy propagation and dead code elimination within a 'block'
 * the body of an action or control (TODO -- extend to parsers/states).  Within the
 * block it tracks all variables defined in the block as well as those defined outside the
 * block that are used in the block, though only those local to the block can be eliminated
 */

class DoLocalCopyPropagation::ElimDead : public Transform {
    /* Traversal to eliminate dead locals in a block after copyprop
     * we run this over just the block (body and declarations) after copyprop
     * of the block, so it only removes those vars declared in the block */
    DoLocalCopyPropagation &self;
    const IR::Node *preorder(IR::Declaration_Variable *var) override {
        if (auto local = ::P4::getref(self.available, var->name)) {
            if (local->local && !local->live) {
                LOG3("  removing dead local " << var->name);
                return nullptr;
            }
        }
        return var;
    }
    const IR::Statement *postorder(IR::BaseAssignmentStatement *as) override {
        if (auto dest = lvalue_out(as->left)->to<IR::PathExpression>()) {
            if (auto var = ::P4::getref(self.available, dest->path->name)) {
                if (var->local && !var->live) {
                    LOG3("  removing dead assignment to " << dest->path->name);
                    if (self.hasSideEffects(as->right, getChildContext()))
                        return makeSideEffectStatement(as->right);
                    return nullptr;
                } else if (var->local) {
                    LOG6("  not removing live assignment to " << dest->path->name);
                } else {
                    LOG6("  not removing assignment to non-local " << dest->path->name);
                }
            }
        }
        return as;
    }
    IR::IfStatement *postorder(IR::IfStatement *s) override {
        if (s->ifTrue == nullptr) {
            /* can't leave ifTrue == nullptr, as that will fail validation -- fold away
             * the if statement as needed */
            if (s->ifFalse == nullptr) {
                if (!self.hasSideEffects(s->condition, getChildContext())) {
                    return nullptr;
                } else {
                    s->ifTrue = new IR::EmptyStatement(s->srcInfo);
                    return s;
                }
            }
            s->ifTrue = s->ifFalse;
            s->ifFalse = nullptr;
            s->condition = new IR::LNot(s->condition);
        }
        return s;
    }
    IR::BlockStatement *postorder(IR::BlockStatement *blk) override {
        if (!blk->components.empty()) return blk;
        if (getParent<IR::BlockStatement>() || getParent<IR::IndexedVector<IR::StatOrDecl>>())
            return nullptr;
        return blk;
    }
    IR::P4Table *preorder(IR::P4Table *tbl) override {
        auto table = self.tables->find(tbl->name);
        if (table != self.tables->end() && table->second.apply_count == 0) {
            LOG3("  removing dead local table " << tbl->name);
            return nullptr;
        }
        return tbl;
    }
    IR::P4Action *preorder(IR::P4Action *act) override {
        auto action = self.actions->find(act->name);
        if (action != self.actions->end() && action->second.apply_count == 0) {
            LOG3("  removing dead local action " << act->name);
            return nullptr;
        }
        return act;
    }

 public:
    explicit ElimDead(DoLocalCopyPropagation &self) : self(self) { setCalledBy(&self); }
};

class DoLocalCopyPropagation::RewriteTableKeys : public Transform {
    /* When doing copyprop on a control, if we have values available at a table.apply
     * call and these values are used in the key of the table, we want to propagate them
     * into the table (replacing the write keys).  We can't do that while we're visiting
     * the control apply body, so we record the need for it in the TableInfo, and then
     * after finishing the control apply body, we call this pass to re-transform the
     * local tables in the control based on what is in their TableInfo. */
    DoLocalCopyPropagation &self;
    TableInfo *table = nullptr;
    IR::P4Table *preorder(IR::P4Table *tbl) {
        BUG_CHECK(table == nullptr, "corrupt internal state");
        table = &(*self.tables)[tbl->name];
        LOG3("RewriteTableKeys for table " << tbl->name);
        return tbl;
    }
    IR::P4Table *postorder(IR::P4Table *tbl) {
        BUG_CHECK(table == &(*self.tables)[tbl->name], "corrupt internal state");
        table = nullptr;
        return tbl;
    }
    const IR::Expression *preorder(IR::Expression *exp) {
        visitAgain();
        if (!table) return exp;
        if (auto name = expr_name(exp)) {
            const Visitor::Context *ctxt = nullptr;
            if (isInContext<IR::KeyElement>(ctxt) && ctxt->child_index == 0) {
                auto key = table->key_remap.find(name);
                if (key != table->key_remap.end()) {
                    LOG4("  rewriting key " << name << " : " << key->second);
                    return key->second;
                }
            }
        }
        return exp;
    }

 public:
    explicit RewriteTableKeys(DoLocalCopyPropagation &self) : self(self) { setCalledBy(&self); }
};

void DoLocalCopyPropagation::flow_merge(Visitor &a_) {
    auto &a = dynamic_cast<DoLocalCopyPropagation &>(a_);
    BUG_CHECK(working == a.working, "inconsitent DoLocalCopyPropagation state on merge");
    LOG8("flow_merge " << a.uid << " into " << uid);
    unreachable &= a.unreachable;
    for (auto &var : available) {
        if (auto merge = ::P4::getref(a.available, var.first)) {
            if (merge->val != var.second.val) {
                if (var.second.val) {
                    LOG4("    dropping " << var.first << " = " << var.second.val
                                         << " in flow_merge");
                }
                var.second.val = nullptr;
            }
            if (merge->live) var.second.live = true;
        } else {
            if (var.second.val)
                LOG4("    dropping " << var.first << " = " << var.second.val << " in flow_merge");
            var.second.val = nullptr;
        }
    }
    need_key_rewrite |= a.need_key_rewrite;
}
void DoLocalCopyPropagation::flow_copy(ControlFlowVisitor &a_) {
    auto &a = dynamic_cast<DoLocalCopyPropagation &>(a_);
    BUG_CHECK(working == a.working, "inconsistent DoLocalCopyPropagation state on copy");
    LOG8("flow_copy " << a.uid << " into " << uid);
    unreachable = a.unreachable;
    available = a.available;
    need_key_rewrite = a.need_key_rewrite;
    BUG_CHECK(inferForTable == a.inferForTable,
              "inconsistent DoLocalCopyPropagation state on copy");
    BUG_CHECK(inferForFunc == a.inferForFunc, "inconsistent DoLocalCopyPropagation state on copy");
}
bool DoLocalCopyPropagation::operator==(const ControlFlowVisitor &a_) const {
    auto &a = dynamic_cast<const DoLocalCopyPropagation &>(a_);
    BUG_CHECK(working == a.working, "inconsistent DoLocalCopyPropagation state on ==");
    if (unreachable != a.unreachable) return false;
    auto it = a.available.begin();
    for (auto &var : available) {
        if (it == a.available.end()) return false;
        if (var.first != it->first) return false;
        if (var.second.local != it->second.local) return false;
        if (var.second.live != it->second.live) return false;
        if (var.second.val != it->second.val) return false;
        ++it;
    }
    if (it != a.available.end()) return false;
    if (need_key_rewrite != a.need_key_rewrite) return false;
    return true;
}

/// test to see if names denote overlapping locations
bool DoLocalCopyPropagation::name_overlap(cstring name1, cstring name2) {
    if (name1 == name2) return true;
    if (name1.startsWith(name2) && strchr(".[", name1.get(name2.size()))) return true;
    if (name2.startsWith(name1) && strchr(".[", name2.get(name1.size()))) return true;
    return false;
}

bool DoLocalCopyPropagation::isHeaderUnionIsValid(const IR::Expression *e) {
    auto mce = e->to<IR::MethodCallExpression>();
    if (!mce) return false;
    auto mi = P4::MethodInstance::resolve(mce, this, typeMap);
    if (auto bm = mi->to<P4::BuiltInMethod>()) {
        if (bm->name == "isValid" && bm->expr->arguments->size() == 0) {
            if (bm->appliedTo->type->is<IR::Type_HeaderUnion>()) return true;
        }
    }
    return false;
}

void DoLocalCopyPropagation::forOverlapAvail(cstring name,
                                             std::function<void(cstring, VarInfo *)> fn) {
    for (const char *pfx = name.c_str(); *pfx; pfx += strspn(pfx, ".[")) {
        pfx += strcspn(pfx, ".[");
        auto it = available.find(name.before(pfx));
        if (it != available.end()) fn(it->first, &it->second);
    }
    for (auto it = available.upper_bound(name); it != available.end(); ++it) {
        if (!it->first.startsWith(name) || !strchr(".[", it->first.get(name.size()))) break;
        fn(it->first, &it->second);
    }
}

void DoLocalCopyPropagation::dropValuesUsing(cstring name) {
    LOG6("dropValuesUsing(" << name << ")");
    for (auto &var : available) {
        LOG7("  checking " << var.first << " = " << var.second.val);
        if (name_overlap(var.first, name)) {
            LOG4("   dropping " << (var.second.val ? "" : "(nop) ") << "as " << name
                                << " is being assigned to");
            var.second.val = nullptr;
        } else if (var.second.val && exprUses(var.second.val, name)) {
            LOG4("   dropping " << (var.second.val ? "" : "(nop) ") << var.first << " as it uses "
                                << name);
            var.second.val = nullptr;
        }
    }
}

void DoLocalCopyPropagation::visit_local_decl(const IR::Declaration_Variable *var) {
    LOG4("Visiting " << var);
    auto [it, inserted] = available.emplace(var->name, VarInfo());
    BUG_CHECK(inserted, "duplicate var declaration for %s", var->name);
    auto &local = it->second;
    local.local = true;
    if (var->initializer) {
        if (!hasSideEffects(var->initializer, getChildContext())) {
            LOG3("  saving init value for " << var->name << ": " << var->initializer);
            local.val = var->initializer;
        } else {
            local.live = true;
        }
    }
}

const IR::Node *DoLocalCopyPropagation::postorder(IR::Declaration_Variable *var) {
    if (working)
        visit_local_decl(var);
    else
        visitAgain();
    return var;
}

IR::Expression *DoLocalCopyPropagation::preorder(IR::Expression *exp) {
    visitAgain();
    return exp;
}

const IR::Expression *DoLocalCopyPropagation::copyprop_name(cstring name,
                                                            const Util::SourceInfo &srcInfo) {
    if (!name) return nullptr;
    if (inferForTable) {
        const Visitor::Context *ctxt = nullptr;
        if (isInContext<IR::KeyElement>(ctxt) && ctxt->child_index == 0)
            inferForTable->keyreads.insert(name);
    }
    if (!working) return nullptr;
    LOG6("  copyprop_name(" << name << ")" << (isWrite() ? " (write)" : ""));
    if (isWrite()) {
        dropValuesUsing(name);
        if (inferForFunc) {
            inferForFunc->is_first_write_insert = inferForFunc->writes.insert(name).second;
        }
        if (isRead() || isInContext<IR::MethodCallExpression>()) {
            /* If this is being used as an 'out' param of a method call, its not really
             * read, but we can't dead-code eliminate it without eliminating the entire
             * call, so we mark it as live.  Unfortunate as we then won't dead-code
             * remove other assignmnents. */
            forOverlapAvail(name, [name](cstring, VarInfo *var) {
                LOG4("  using " << name << " in read-write");
                var->live = true;
            });
            if (inferForFunc) inferForFunc->reads.insert(name);
        }
        return nullptr;
    }
    if (auto var = ::P4::getref(available, name)) {
        if (var->val) {
            if (policy(getChildContext(), var->val, this)) {
                LOG3("  propagating value for " << name << ": " << var->val);
                CopySrcInfo copy(srcInfo);
                return var->val->apply(copy);
            }
            LOG3("  policy rejects propagation of " << name << ": " << var->val);
        } else {
            LOG4("  using " << name << " with no propagated value");
        }
        var->live = true;
    }
    forOverlapAvail(name, [name](cstring, VarInfo *var) {
        LOG4("  using part of " << name);
        var->live = true;
    });
    if (inferForFunc) inferForFunc->reads.insert(name);
    return nullptr;
}

const IR::Expression *DoLocalCopyPropagation::postorder(IR::PathExpression *path) {
    auto rv = copyprop_name(path->path->name, path->srcInfo);
    return rv ? rv : path;
}

const IR::Expression *DoLocalCopyPropagation::preorder(IR::Member *member) {
    visitAgain();
    if (auto name = expr_name(member)) {
        prune();
        if (auto rv = copyprop_name(name, member->srcInfo)) return rv;
    }
    return member;
}

const IR::Expression *DoLocalCopyPropagation::preorder(IR::ArrayIndex *arr) {
    visitAgain();
    if (auto name = expr_name(arr)) {
        prune();
        if (auto rv = copyprop_name(name, arr->srcInfo)) return rv;
    }
    return arr;
}

IR::Statement *DoLocalCopyPropagation::preorder(IR::Statement *s) {
    visitAgain();
    return s;
}

const IR::Node *DoLocalCopyPropagation::preorder(IR::BaseAssignmentStatement *as) {
    visitAgain();
    if (!working) return as;
    // visit the source subtree first, before the destination subtree
    // make sure child indexes are set properly so we can detect writes -- these are the
    // extra arguments to 'visit' in order to make introspection vis the Visitor::Context
    // work.  Normally this is all managed by the auto-generated visit_children methods,
    // but since we're overriding that here AND P4WriteContext cares about it (that's how
    // it determines whether something is a write or a read), it needs to be correct
    // This is error-prone and fragile
    visit(as->right, "right", 1);
    visit(as->left, "left", 0);
    prune();
    return as->apply_visitor_postorder(*this);
}

IR::AssignmentStatement *DoLocalCopyPropagation::postorder(IR::AssignmentStatement *as) {
    if (as->left->equiv(*as->right)) {
        LOG3("  removing noop assignment " << *as);
        if (inferForFunc && inferForFunc->is_first_write_insert) {
            inferForFunc->writes.erase(expr_name(as->left));
            inferForFunc->is_first_write_insert = false;
        }
        return nullptr;
    }
    // FIXME -- if as->right is an uninitialized value, we could legally eliminate this
    // assignment, which would simplify (and minimize) the code.  This could be a separate
    // pass, but doing it here would mesh nicely with ElimDead, and we have (or could
    // compute) the necessary info for local vars.
    if (!working) return as;
    if (auto dest = expr_name(as->left)) {
        if (!hasSideEffects(as->right, getChildContext())) {
            if (as->right->is<IR::ListExpression>()) {
                /* FIXME -- List Expressions need to be turned into constructor calls before
                 * we can copyprop them */
                return as;
            }
            if (exprUses(as->right, dest)) {
                /* can't propagate the value as it is defined in terms of itself.
                 * FIXME -- we could propagate if we introduced a new temp, but that
                 * may make things worse rather than better */
                return as;
            }
            LOG3("  saving value for " << dest << ": " << as->right);
            available[dest].val = as->right;
        } else {
            LOG3("Can't copyprop " << as->right << " due to side effects");
        }
    } else {
        LOG3("dest of assignment is " << as->left << " so skipping");
    }
    return as;
}

void DoLocalCopyPropagation::LoopPrepass::postorder(const IR::BaseAssignmentStatement *as) {
    if (auto dest = expr_name(as->left)) self.dropValuesUsing(dest);
}

IR::IfStatement *DoLocalCopyPropagation::postorder(IR::IfStatement *s) {
    if (s->ifTrue == nullptr) {
        /* can't leave ifTrue == nullptr, as that will fail validation -- fold away
         * the if statement as needed. We do not apply ElimDead as we don't want to
         * modify other statements in the midst of analysis. */
        if (s->ifFalse == nullptr) {
            if (!hasSideEffects(s->condition, getChildContext())) {
                return nullptr;
            } else {
                s->ifTrue = new IR::EmptyStatement();
            }
        } else {
            s->ifTrue = s->ifFalse;
            s->ifFalse = nullptr;
            s->condition = new IR::LNot(s->condition);
        }
    }
    return s;
}

IR::ForStatement *DoLocalCopyPropagation::preorder(IR::ForStatement *s) {
    visit(s->init, "init");
    s->apply(LoopPrepass(*this), getContext());
    ControlFlowVisitor::SaveGlobal outer(*this, "-BREAK-"_cs, "-CONTINUE-"_cs);
    visit(s->condition, "condition");
    visit(s->body, "body");
    flow_merge_global_from("-CONTINUE-"_cs);
    visit(s->updates, "updates");
    flow_merge_global_from("-BREAK-"_cs);
    prune();
    return s;
}

IR::ForInStatement *DoLocalCopyPropagation::preorder(IR::ForInStatement *s) {
    visit(s->decl, "decl", 0);
    visit(s->collection, "collection", 2);
    s->apply(LoopPrepass(*this), getContext());
    ControlFlowVisitor::SaveGlobal outer(*this, "-BREAK-"_cs, "-CONTINUE-"_cs);
    visit(s->ref, "ref", 1);
    visit(s->body, "body", 3);
    flow_merge_global_from("-CONTINUE-"_cs);
    flow_merge_global_from("-BREAK-"_cs);
    prune();
    return s;
}

bool isAsync(const IR::Vector<IR::Method> methods, cstring callee, cstring caller) {
    if (callee.startsWith(".")) callee = callee.substr(1);
    for (auto *m : methods) {
        if (m->name != callee) continue;
        auto sync = m->getAnnotation(IR::Annotation::synchronousAnnotation);
        if (!sync) return true;
        for (auto m : sync->getExpr()) {
            auto mname = m->to<IR::PathExpression>();
            if (mname && mname->path->name == caller) return false;
        }
        return true;
    }
    return false;  // can't find the method, so assume synchronous
}

IR::MethodCallExpression *DoLocalCopyPropagation::postorder(IR::MethodCallExpression *mc) {
    if (!working) return mc;
    auto *mi = MethodInstance::resolve(mc, this, typeMap, true);
    if (auto mem = mc->method->to<IR::Member>()) {
        if (auto obj = expr_name(mem->expr)) {
            auto tbl = tables->find(obj);
            if (tbl != tables->end()) {
                LOG3("table apply method call " << mc->method);
                apply_table(&tbl->second);
                return mc;
            } else if (auto em = mi->to<ExternMethod>()) {
                auto ext = em->actualExternType;
                auto name = obj + '.' + mem->member;
                auto mtd = methods->find(name);
                if (mtd != methods->end()) {
                    LOG3("concrete method call " << name);
                    apply_function(&mtd->second);
                    return mc;
                } else if (ext && (ext->name == "packet_in" || ext->name == "packet_out")) {
                    LOG3(ext->name << '.' << mem->member << " call");
                    // we currently do no track the liveness of packet_in/out objects
                    return mc;
                } else {
                    // extern method call might trigger various concrete implementation
                    // method in the object, so act as if all of them are applied
                    // FIXME -- Could it invoke methods on other objects or otherwise affect
                    // global values?  Not clear -- we probably need a hook for backends
                    // to provide per-extern flow info to this (and other) frontend passes.
                    LOG3("extern method call " << name);
                    for (auto *n : em->mayCall()) {
                        if (auto *method = ::P4::getref(*methods, obj + '.' + n->getName())) {
                            LOG4("  might call " << obj << '.' << n->getName());
                            apply_function(method);
                        }
                    }
                    return mc;
                }
            } else if (mem->expr->type->is<IR::Type_Header>() ||
                       mem->expr->type->is<IR::Type_HeaderUnion>()) {
                if (mem->member == "isValid") {
                    forOverlapAvail(obj, [obj](cstring, VarInfo *var) {
                        LOG4("  using " << obj << " (isValid)");
                        var->live = true;
                    });
                    if (inferForFunc) inferForFunc->reads.insert(obj);
                } else {
                    BUG_CHECK(mem->member == "setValid" || mem->member == "setInvalid",
                              "Unexpected header method %s", mem->member);
                    LOG3("header method call " << mc->method << " writes to " << obj);
                    dropValuesUsing(obj);
                    if (inferForFunc) inferForFunc->writes.insert(obj);
                }
                return mc;
            } else if (mem->expr->type->is<IR::Type_Array>()) {
                BUG_CHECK(mem->member == "push_front" || mem->member == "pop_front",
                          "Unexpected stack method %s", mem->member);
                dropValuesUsing(obj);
                forOverlapAvail(obj, [obj](cstring, VarInfo *var) {
                    LOG4("  using " << obj << " (push/pop)");
                    var->live = true;
                });
                if (inferForFunc) {
                    inferForFunc->reads.insert(obj);
                    inferForFunc->writes.insert(obj);
                }
                return mc;
            }
        }
    } else if (auto fn = mc->method->to<IR::PathExpression>()) {
        auto act = actions->find(fn->path->name);
        if (act != actions->end()) {
            LOG3("action method call " << mc->method);
            apply_function(&act->second);
            return mc;
        } else if (mi->is<P4::ExternFunction>()) {
            LOG3("extern function call " << mc->method);
            // assume it has no side-effects on anything not explicitly passed to it?
            // maybe should have annotations if it does
            return mc;
        }
    }
    LOG3("unknown method call " << mc->method << " clears all nonlocal saved values");
    for (auto &var : available) {
        if (!var.second.local) {
            LOG7("    may access non-local " << var.first);
            var.second.val = nullptr;
            var.second.live = true;
            if (inferForFunc) {
                inferForFunc->reads.insert(var.first);
                inferForFunc->writes.insert(var.first);
            }
        }
    }
    return mc;
}

// FIXME -- this duplicates much of what is in the above method -- factor things better
void DoLocalCopyPropagation::LoopPrepass::postorder(const IR::MethodCallExpression *mc) {
    auto *mi = MethodInstance::resolve(mc, &self, self.typeMap, true);
    if (auto mem = mc->method->to<IR::Member>()) {
        if (auto obj = expr_name(mem->expr)) {
            auto tbl = self.tables->find(obj);
            if (tbl != self.tables->end()) {
                LOG3("loop prepass table apply method call " << mc->method);
                apply_table(&tbl->second);
                return;
            } else if (auto em = mi->to<ExternMethod>()) {
                auto ext = em->actualExternType;
                auto name = obj + '.' + mem->member;
                auto mtd = self.methods->find(name);
                if (mtd != self.methods->end()) {
                    LOG3("loop prepass concrete method call " << name);
                    apply_function(&mtd->second);
                    return;
                } else if (ext && (ext->name == "packet_in" || ext->name == "packet_out")) {
                    LOG3("loop prepass " << ext->name << '.' << mem->member << " call");
                    // we currently do no track the liveness of packet_in/out objects
                    return;
                } else {
                    // extern method call might trigger various concrete implementation
                    // method in the object, so act as if all of them are applied
                    // FIXME -- Could it invoke methods on other objects or otherwise affect
                    // global values?  Not clear -- we probably need a hook for backends
                    // to provide per-extern flow info to this (and other) frontend passes.
                    LOG3("loop prepass extern method call " << name);
                    for (auto *n : em->mayCall()) {
                        if (auto *method = P4::getref(*self.methods, obj + '.' + n->getName())) {
                            LOG4("  might call " << obj << '.' << n->getName());
                            apply_function(method);
                        }
                    }
                    return;
                }
            } else if (mem->expr->type->is<IR::Type_Header>() ||
                       mem->expr->type->is<IR::Type_HeaderUnion>()) {
                BUG_CHECK(mem->member == "setValid" || mem->member == "setInvalid",
                          "Unexpected header method %s", mem->member);
                LOG3("loop prepass header method call " << mc->method << " writes to " << obj);
                self.dropValuesUsing(obj);
                return;
            } else if (mem->expr->type->is<IR::Type_Array>()) {
                BUG_CHECK(mem->member == "push_front" || mem->member == "pop_front",
                          "Unexpected stack method %s", mem->member);
                self.dropValuesUsing(obj);
                return;
            }
        }
    } else if (auto fn = mc->method->to<IR::PathExpression>()) {
        auto act = self.actions->find(fn->path->name);
        if (act != self.actions->end()) {
            LOG3("loop prepass action method call " << mc->method);
            apply_function(&act->second);
            return;
        } else if (mi->is<P4::ExternFunction>()) {
            LOG3("self.extern function call " << mc->method);
            // assume it has no side-effects on anything not explicitly passed to it?
            // maybe should have annotations if it does
            return;
        }
    }
    LOG3("loop prepass unknown method call " << mc->method << " clears all nonlocal saved values");
    for (auto &var : self.available) {
        if (!var.second.local) {
            LOG7("    may access non-local " << var.first);
            var.second.val = nullptr;
        }
    }
    return;
}

IR::P4Action *DoLocalCopyPropagation::preorder(IR::P4Action *act) {
    visitOnce();
    BUG_CHECK(!working && !inferForFunc && available.empty(), "corrupt internal data struct");
    working = true;
    inferForFunc = &(*actions)[act->name];
    LOG2("DoLocalCopyPropagation working on action " << act->name);
    LOG4(act);
    return act;
}

IR::P4Action *DoLocalCopyPropagation::postorder(IR::P4Action *act) {
    LOG5("DoLocalCopyPropagation before ElimDead " << act->name);
    LOG5(act);
    BUG_CHECK(inferForFunc == &(*actions)[act->name], "corrupt internal data struct");
    act->body = act->body->apply(ElimDead(*this), getChildContext())->to<IR::BlockStatement>();
    working = false;
    available.clear();
    LOG3("DoLocalCopyPropagation finished action " << act->name);
    LOG4("reads=" << inferForFunc->reads << " writes=" << inferForFunc->writes);
    LOG4(act);
    inferForFunc = nullptr;
    return act;
}

IR::Function *DoLocalCopyPropagation::preorder(IR::Function *fn) {
    visitOnce();
    BUG_CHECK(!working && !inferForFunc && available.empty(), "corrupt internal data struct");
    cstring name;
    if (auto decl = findContext<IR::Declaration_Instance>())
        // abstract function implementation
        name = decl->name + '.' + fn->name;
    else
        name = fn->name;
    working = true;
    inferForFunc = &(*methods)[name];
    LOG2("DoLocalCopyPropagation working on function " << name);
    LOG4(fn);
    return fn;
}

IR::Function *DoLocalCopyPropagation::postorder(IR::Function *fn) {
    cstring name;
    if (auto decl = findContext<IR::Declaration_Instance>())
        // abstract function implementation
        name = decl->name + '.' + fn->name;
    else
        name = fn->name;
    LOG5("DoLocalCopyPropagation before ElimDead " << name);
    LOG5(fn);
    BUG_CHECK(inferForFunc == &(*methods)[name], "corrupt internal data struct");
    fn->body = fn->body->apply(ElimDead(*this), getChildContext())->to<IR::BlockStatement>();
    working = false;
    available.clear();
    LOG3("DoLocalCopyPropagation finished function " << name);
    LOG4("reads=" << inferForFunc->reads << " writes=" << inferForFunc->writes);
    LOG4(fn);
    inferForFunc = nullptr;
    return fn;
}

IR::P4Control *DoLocalCopyPropagation::preorder(IR::P4Control *ctrl) {
    visitOnce();
    BUG_CHECK(!working && available.empty(), "corrupt internal data struct");
    visit(ctrl->type, "type");
    visit(ctrl->constructorParams, "constructorParams");
    visit(ctrl->controlLocals, "controlLocals");
    if (working || !available.empty()) BUG("corrupt internal data struct");
    working = true;
    LOG2("DoLocalCopyPropagation working on control " << ctrl->name);
    LOG4(ctrl);
    need_key_rewrite = false;
    for (auto local : ctrl->controlLocals)
        if (auto var = local->to<IR::Declaration_Variable>()) visit_local_decl(var);
    visit(ctrl->body, "body");
    if (need_key_rewrite) ctrl->controlLocals = *ctrl->controlLocals.apply(RewriteTableKeys(*this));
    if (!elimUnusedTables) {
        for (auto local : ctrl->controlLocals) {
            if (auto *act = local->to<IR::P4Action>()) {
                auto &act_info = actions->at(act->name);
                if (act_info.apply_count == 0) {
                    // unused action -- preserve any unused locals it references
                    apply_function(&act_info);
                }
            } else if (auto *tbl = local->to<IR::P4Table>()) {
                auto &tbl_info = tables->at(tbl->name);
                if (tbl_info.apply_count == 0) {
                    // unused table -- preserve any unused locals it references
                    apply_table(&tbl_info);
                }
            }
        }
    }
    LOG5("DoLocalCopyPropagation before ElimDead " << ctrl->name);
    LOG5(ctrl);
    ctrl->controlLocals = *ctrl->controlLocals.apply(ElimDead(*this), getChildContext());
    ctrl->body = ctrl->body->apply(ElimDead(*this), getChildContext())->to<IR::BlockStatement>();
    working = false;
    available.clear();
    LOG3("DoLocalCopyPropagation finished control " << ctrl->name);
    LOG4(ctrl);
    prune();
    return ctrl;
}

void DoLocalCopyPropagation::apply_function(DoLocalCopyPropagation::FuncInfo *act) {
    LOG7("apply_function reads=" << act->reads << " writes=" << act->writes);
    ++act->apply_count;
    for (auto write : act->writes) dropValuesUsing(write);
    for (auto read : act->reads)
        forOverlapAvail(read, [](cstring, VarInfo *var) { var->live = true; });
    if (inferForFunc) {
        inferForFunc->writes.insert(act->writes.begin(), act->writes.end());
        inferForFunc->reads.insert(act->reads.begin(), act->reads.end());
    }
}

void DoLocalCopyPropagation::LoopPrepass::apply_function(DoLocalCopyPropagation::FuncInfo *act) {
    LOG7("loop prepass apply_function writes=" << act->writes);
    for (auto write : act->writes) self.dropValuesUsing(write);
}

void DoLocalCopyPropagation::apply_table(DoLocalCopyPropagation::TableInfo *tbl) {
    ++tbl->apply_count;
    std::unordered_set<cstring> remaps_seen;
    for (auto key : tbl->keyreads) {
        forOverlapAvail(key, [&remaps_seen, key, tbl, this](cstring vname, VarInfo *var) {
            remaps_seen.insert(vname);
            if (var->val && lvalue_out(var->val)->is<IR::PathExpression>()) {
                if (tbl->apply_count > 1 &&
                    (!tbl->key_remap.count(vname) || !tbl->key_remap.at(vname)->equiv(*var->val))) {
                    LOG3("  different values used in different applies for key " << key);
                    tbl->key_remap.erase(vname);
                    var->live = true;
                } else if (policy(getChildContext(), var->val, this)) {
                    LOG3("  will propagate value into table key " << vname << ": " << var->val);
                    tbl->key_remap.emplace(vname, var->val);
                    need_key_rewrite = true;
                } else {
                    LOG3("  policy prevents propagation of value into table key " << vname << ": "
                                                                                  << var->val);
                    var->live = true;
                }
            } else if (var->val && lvalue_out(var->val)->is<IR::MethodCallExpression>()) {
                if (hasSideEffects(lvalue_out(var->val), getChildContext())) {
                    LOG3("  cannot propagate expression with side effect into table key "
                         << vname << ": " << var->val);
                    var->live = true;
                } else if (isHeaderUnionIsValid(lvalue_out(var->val))) {
                    // isValid() on a header union must be handled by the flattenHeaderUnion
                    // pass to lower it to isValid() on a header. Therefore we cannot propagate
                    // it into a table key.
                    LOG3("  cannot propagate isValid() for header union into table key "
                         << vname << ": " << var->val);
                    var->live = true;
                } else if (tbl->apply_count > 1 && (!tbl->key_remap.count(vname) ||
                                                    !tbl->key_remap.at(vname)->equiv(*var->val))) {
                    LOG3("  different values used in different applies for key " << key);
                    tbl->key_remap.erase(vname);
                    var->live = true;
                } else if (policy(getChildContext(), var->val, this)) {
                    LOG3("  will propagate value into table key " << vname << ": " << var->val);
                    tbl->key_remap.emplace(vname, var->val);
                    need_key_rewrite = true;
                } else {
                    LOG3("  policy prevents propagation of value into table key " << vname << ": "
                                                                                  << var->val);
                    var->live = true;
                }
            } else {
                tbl->key_remap.erase(key);
                LOG4("  table using "
                     << key << " with "
                     << (var->val ? "value too complex for key" : "no propagated value"));
                var->live = true;
            }
        });
    }
    for (auto it = tbl->key_remap.begin(); it != tbl->key_remap.end();) {
        if (remaps_seen.count(it->first) == 0) {
            LOG3("  no value used for some applies for key " << it->first);
            it = tbl->key_remap.erase(it);
        } else {
            ++it;
        }
    }
    for (auto action : tbl->actions) apply_function(&(*actions)[action]);
}

void DoLocalCopyPropagation::LoopPrepass::apply_table(DoLocalCopyPropagation::TableInfo *tbl) {
    for (auto action : tbl->actions) apply_function(&(*self.actions)[action]);
}

IR::P4Table *DoLocalCopyPropagation::preorder(IR::P4Table *tbl) {
    visitOnce();
    BUG_CHECK(!inferForTable, "corrupt internal data struct");
    inferForTable = &(*tables)[tbl->name];
    inferForTable->keyreads.clear();
    for (auto ale : tbl->getActionList()->actionList)
        inferForTable->actions.insert(ale->getPath()->name);
    return tbl;
}

IR::P4Table *DoLocalCopyPropagation::postorder(IR::P4Table *tbl) {
    BUG_CHECK(inferForTable == &(*tables)[tbl->name], "corrupt internal data struct");
    LOG4("table " << tbl->name << " reads=" << inferForTable->keyreads
                  << " actions=" << inferForTable->actions);
    inferForTable = nullptr;
    return tbl;
}

const IR::P4Parser *DoLocalCopyPropagation::postorder(IR::P4Parser *parser) {
    BUG_CHECK(!working && available.empty(), "corrupt internal data struct");
    working = true;
    LOG2("DoLocalCopyPropagation working on parser " << parser->name);
    visit(parser->parserLocals, "parserLocals");  // visit these again with working==true
    for (auto *state : parser->states) apply_function(&(*states)[state->name]);
    auto *rv = parser->apply(ElimDead(*this), getChildContext());
    working = false;
    available.clear();
    return rv;
}

IR::ParserState *DoLocalCopyPropagation::preorder(IR::ParserState *state) {
    visitOnce();
    BUG_CHECK(!working && available.empty(), "corrupt internal data struct");
    working = true;
    LOG2("DoLocalCopyPropagation working on parser state " << state->name);
    LOG4(state);
    inferForFunc = &(*states)[state->name];
    return state;
}

IR::ParserState *DoLocalCopyPropagation::postorder(IR::ParserState *state) {
    BUG_CHECK(working && inferForFunc == &(*states)[state->name], "corrupt internal data struct");
    LOG5("DoLocalCopyPropagation before ElimDead " << state->name);
    LOG5(state);
    state->components = *state->components.apply(ElimDead(*this), getChildContext());
    working = false;
    inferForFunc = nullptr;
    available.clear();
    LOG3("DoLocalCopyPropagation finished parser state " << state->name);
    LOG4(state);
    return state;
}

// Reset the state of internal data structures after traversing IR,
// needed for this pass to function correctly when used in a PassRepeated
void DoLocalCopyPropagation::end_apply(const IR::Node *node) {
    uid = uid_ctr = 0;  // reset uids

    // clear maps
    available.clear();
    tables->clear();
    actions->clear();
    methods->clear();
    states->clear();
    // reset pointers
    inferForFunc = nullptr;
    inferForTable = nullptr;
    // reset flags
    need_key_rewrite = false;
    elimUnusedTables = false;
    working = false;

    Transform::end_apply(node);
}

}  // namespace P4
