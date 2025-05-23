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

#ifndef FRONTENDS_COMMON_RESOLVEREFERENCES_REFERENCEMAP_H_
#define FRONTENDS_COMMON_RESOLVEREFERENCES_REFERENCEMAP_H_

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "frontends/common/programMap.h"
#include "ir/ir.h"
#include "ir/visitor.h"
#include "lib/cstring.h"

namespace P4 {

class NameGenerator {
 public:
    virtual cstring newName(std::string_view base) = 0;
    virtual ~NameGenerator() = default;
};

// replacement for ReferenceMap NameGenerator to make it easier to remove uses of refMap
class MinimalNameGenerator : public NameGenerator, public Inspector {
    /// All names used in the program. Key is a name, value represents how many times
    /// this name was used as a base for newly generated unique names.
    absl::flat_hash_map<cstring, int, Util::Hash> usedNames;
    void postorder(const IR::Path *p) override { usedName(p->name.name); }
    void postorder(const IR::Type_Declaration *t) override { usedName(t->name.name); }
    void postorder(const IR::Declaration *d) override { usedName(d->name.name); }

 public:
    MinimalNameGenerator();
    void usedName(cstring name) { usedNames.emplace(name, 0); }
    explicit MinimalNameGenerator(const IR::Node *root) : MinimalNameGenerator() {
        root->apply(*this);
    }

    /// Generate a name from @p base that does not appear in usedNames.
    cstring newName(std::string_view base) override;
};

// FIXME -- temp common base class to allow use of ReferenceMap or ResolutionContext
// interchangeably when looking up declarations.  This should go away once the refMap does
class DeclarationLookup {
 public:
    virtual ~DeclarationLookup() = default;
    virtual const IR::IDeclaration *getDeclaration(const IR::Path *,
                                                   bool notNull = false) const = 0;
    virtual const IR::IDeclaration *getDeclaration(const IR::This *,
                                                   bool notNull = false) const = 0;
};

/// Class used to encode maps from paths to declarations.
class ReferenceMap final : public ProgramMap, public NameGenerator, public DeclarationLookup {
    /// If `isv1` is true, then the map is for a P4_14 program
    /// (possibly translated into P4_16).
    bool isv1;

    /// Maps paths in the program to declarations.
    absl::flat_hash_map<const IR::Path *, const IR::IDeclaration *, Util::Hash> pathToDeclaration;

    /// Set containing all declarations in the program.
    absl::flat_hash_set<const IR::IDeclaration *, Util::Hash> used;

    /// Map from `This` to declarations (an experimental feature).
    absl::flat_hash_map<const IR::This *, const IR::IDeclaration *, Util::Hash> thisToDeclaration;

    /// All names used in the program. Key is a name, value represents how many times
    /// this name was used as a base for newly generated unique names.
    absl::flat_hash_map<cstring, int, Util::Hash> usedNames;

 public:
    ReferenceMap();
    /// Looks up declaration for @p path. If @p notNull is false, then
    /// failure to find a declaration is an error.
    const IR::IDeclaration *getDeclaration(const IR::Path *path,
                                           bool notNull = false) const override;

    /// Sets declaration for @p path to @p decl.
    void setDeclaration(const IR::Path *path, const IR::IDeclaration *decl);

    /// Looks up declaration for @p pointer. If @p notNull is false,
    /// then failure to find a declaration is an error.
    const IR::IDeclaration *getDeclaration(const IR::This *pointer,
                                           bool notNull = false) const override;

    /// Sets declaration for @p pointer to @p decl.
    void setDeclaration(const IR::This *pointer, const IR::IDeclaration *decl);

    void dbprint(std::ostream &cout) const override;

    /// Set boolean indicating whether map is for a P4_14 program to @p isV1.
    void setIsV1(bool isv1) { this->isv1 = isv1; }
    void setAnyOrder(bool anyOrder) { this->isv1 = anyOrder; }

    /// Generate a name from @p base that fresh for the program.
    cstring newName(std::string_view base) override;

    /// Clear the reference map
    void clear();

    /// @returns @true if this map is for a P4_14 program
    bool isV1() const { return isv1; }

    /// @returns @true if @p decl is used in the program.
    bool isUsed(const IR::IDeclaration *decl) const { return used.count(decl) > 0; }

    /// Indicate that @p name is used in the program.
    void usedName(cstring name) { usedNames.emplace(name, 0); }
};

}  // namespace P4

#endif /* FRONTENDS_COMMON_RESOLVEREFERENCES_REFERENCEMAP_H_ */
