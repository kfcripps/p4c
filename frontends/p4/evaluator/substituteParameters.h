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

/* -*-C++-*- */

#ifndef FRONTENDS_P4_EVALUATOR_SUBSTITUTEPARAMETERS_H_
#define FRONTENDS_P4_EVALUATOR_SUBSTITUTEPARAMETERS_H_

#include "frontends/common/resolveReferences/resolveReferences.h"
#include "frontends/p4/parameterSubstitution.h"
#include "frontends/p4/typeChecking/typeSubstitutionVisitor.h"
#include "ir/ir.h"

namespace P4 {

class SubstituteParameters : public TypeVariableSubstitutionVisitor, public ResolutionContext {
 protected:
    const DeclarationLookup *refMap;     // input
    const ParameterSubstitution *subst;  // input
 public:
    SubstituteParameters(const DeclarationLookup *refMap, const ParameterSubstitution *subst,
                         const TypeVariableSubstitution *tvs)
        : TypeVariableSubstitutionVisitor(tvs), refMap(refMap), subst(subst) {
        CHECK_NULL(subst);
        CHECK_NULL(tvs);
        visitDagOnce = true;
        setName("SubstituteParameters");
        LOG1("Will substitute " << std::endl << subst << bindings);
    }
    using TypeVariableSubstitutionVisitor::postorder;
    const IR::Node *postorder(IR::PathExpression *expr) override;
    const IR::Node *postorder(IR::Type_Name *type) override;
    const IR::Node *postorder(IR::This *t) override;
};

}  // namespace P4

#endif /* FRONTENDS_P4_EVALUATOR_SUBSTITUTEPARAMETERS_H_ */
