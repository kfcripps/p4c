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

#ifndef FRONTENDS_P4_UNUSEDDECLARATIONS_H_
#define FRONTENDS_P4_UNUSEDDECLARATIONS_H_

#include "../common/resolveReferences/resolveReferences.h"
#include "ir/ir.h"
#include "ir/pass_manager.h"
#include "lib/stringify.h"

namespace P4 {

class UnusedDeclarations;

class UsedDeclSet : public IHasDbPrint {
    /// Set containing all declarations in the program.
    absl::flat_hash_set<const IR::IDeclaration *, Util::Hash> usedDecls;

 public:
    bool setUsed(const IR::IDeclaration *decl) { return usedDecls.emplace(decl).second; }

    [[nodiscard]] auto begin() const { return usedDecls.begin(); }

    [[nodiscard]] auto end() const { return usedDecls.end(); }

    void clear() { usedDecls.clear(); }

    void dbprint(std::ostream &cout) const override;

    /// @returns @true if @p decl is used in the program.
    bool isUsed(const IR::IDeclaration *decl) const { return usedDecls.contains(decl); }
};

class RemoveUnusedPolicy {
 public:
    virtual ~RemoveUnusedPolicy() = default;
    /// The policy for removing unused declarations is baked into the pass -- targets can specify
    /// their own subclass of the pass with a changed policy and return that here
    virtual UnusedDeclarations *getUnusedDeclarationsPass(const UsedDeclSet &used,
                                                          bool removeUnused, bool warnUnused,
                                                          bool infoRemoved) const;
};

/// @brief Collects all used declarations into @used set
class CollectUsedDeclarations : public Inspector, ResolutionContext {
    UsedDeclSet &used;

 public:
    explicit CollectUsedDeclarations(UsedDeclSet &used) : used(used) {}

    // We might be invoked in PassRepeated scenario, so the used set should be
    // force cleared.
    Visitor::profile_t init_apply(const IR::Node *node) override {
        auto rv = Inspector::init_apply(node);
        used.clear();

        return rv;
    }

    bool preorder(const IR::KeyElement *ke) override;
    bool preorder(const IR::PathExpression *path) override;
    bool preorder(const IR::Type_Name *type) override;
};

/** @brief Removes unused declarations.
 *
 * The following kinds of nodes are not removed even if they are unreferenced:
 *  - IR::Declaration_MatchKind
 *  - IR::Parameter
 *  - IR::Type_Error
 *  - IR::Type_Extern
 *  - IR::Type_Method
 *  - IR::Type_StructLike
 *  - IR::TypeParameters
 *  - IR::Method if declared in system headers
 *
 * Additionally, IR::Declaration_Instance nodes for extern instances are not
 * removed but still trigger warnings.
 *
 * A compilation warning is emitted when a new node is added to @warned,
 * preventing duplicate warnings per node.
 *
 * @pre Requires an up-to-date ReferenceMap.
 */
class UnusedDeclarations : public Transform, ResolutionContext {
 protected:
    const UsedDeclSet &used;

    /// If true, removes unused instances.
    bool removeUnused;

    /// If true, emits warnings about unused instances.
    bool warnUnused;

    /// If true, emits info messages about removed instances.
    bool infoRemoved;

    /** Logs the following unused elements in @warned:
     *  - unused IR::P4Table nodes
     *  - unused IR::Declaration_Instance nodes
     *
     *  Unused extern instances are not removed but may still trigger
     *  warnings.  The @warned set keeps track of warnings emitted in
     *  previous iterations to avoid emitting duplicate warnings.
     */
    std::set<const IR::Node *> warned;

    /** Stores @node in @warned if:
     *   - @node is an unused declaration,
     *   - @node is not already present in @warned.
     *
     * @return true if @node is added to @warned.
     */
    bool giveWarning(const IR::Node *node);
    const IR::Node *process(const IR::IDeclaration *decl, bool skipWarn = false);
    const IR::Node *warnIfUnused(const IR::Node *node);

    // Prevent direct instantiations of this class.
    friend class RemoveUnusedPolicy;
    UnusedDeclarations(const UsedDeclSet &used, bool removeUnused, bool warnUnused,
                       bool infoRemoved)
        : used(used), removeUnused(removeUnused), warnUnused(warnUnused), infoRemoved(infoRemoved) {
        setName("UnusedDeclarations");
    }

 public:
    using Transform::init_apply;
    using Transform::postorder;
    using Transform::preorder;

    Visitor::profile_t init_apply(const IR::Node *root) override;

    const IR::Node *preorder(IR::P4Control *cont) override;
    const IR::Node *preorder(IR::P4Parser *cont) override;
    const IR::Node *preorder(IR::P4Table *cont) override;
    const IR::Node *preorder(IR::ParserState *state) override;
    const IR::Node *preorder(IR::Type_Enum *type) override;
    const IR::Node *preorder(IR::Type_SerEnum *type) override;

    const IR::Node *preorder(IR::Declaration_Instance *decl) override;
    const IR::Node *preorder(IR::Method *decl) override;

    // The following kinds of nodes are not deleted even if they are unreferenced
    const IR::Node *preorder(IR::Type_Error *type) override {
        prune();
        return type;
    }
    const IR::Node *preorder(IR::Declaration_MatchKind *decl) override {
        prune();
        return decl;
    }
    const IR::Node *preorder(IR::Type_StructLike *type) override {
        visit(type->typeParameters);
        prune();
        return type;
    }
    const IR::Node *preorder(IR::Type_Extern *type) override {
        visit(type->typeParameters);
        prune();
        return type;
    }
    const IR::Node *preorder(IR::Type_Method *type) override {
        visit(type->typeParameters);
        prune();
        return type;
    }
    const IR::Node *preorder(IR::Parameter *param) override;
    const IR::Node *preorder(IR::NamedExpression *ne) override { return ne; }  // never dead
    const IR::Node *preorder(IR::Type_Var *p) override {
        prune();
        return warnIfUnused(p);
    }

    const IR::Node *preorder(IR::Declaration_Variable *decl) override;
    const IR::Node *preorder(IR::Declaration_Constant *decl) override;
    const IR::Node *preorder(IR::Declaration *decl) override { return process(decl); }
    const IR::Node *preorder(IR::Type_Declaration *decl) override { return process(decl); }
};

/** @brief Iterates UnusedDeclarations until convergence.
 */
class RemoveAllUnusedDeclarations : public PassManager {
    UsedDeclSet used;

 public:
    explicit RemoveAllUnusedDeclarations(const RemoveUnusedPolicy &policy,
                                         bool infoRemoved = false) {
        setName("RemoveAllUnusedDeclarations");
        addPasses(
            {new PassRepeated({new CollectUsedDeclarations(used),
                               policy.getUnusedDeclarationsPass(used, true, false, infoRemoved)})});
        setStopOnError(true);
    }
};

/** @brief Emits warnings about unused instances without modifying IR.
 */
class WarnAboutUnusedDeclarations : public PassManager {
    UsedDeclSet used;

 public:
    explicit WarnAboutUnusedDeclarations(const RemoveUnusedPolicy &policy) {
        setName("WarnAboutUnusedDeclarations");
        addPasses({new CollectUsedDeclarations(used),
                   policy.getUnusedDeclarationsPass(used, false, true, false)});
    }
};

}  // namespace P4

#endif /* FRONTENDS_P4_UNUSEDDECLARATIONS_H_ */
