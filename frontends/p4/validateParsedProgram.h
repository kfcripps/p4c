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

#ifndef FRONTENDS_P4_VALIDATEPARSEDPROGRAM_H_
#define FRONTENDS_P4_VALIDATEPARSEDPROGRAM_H_

#include "ir/ir.h"
#include "ir/visitor.h"

namespace P4 {

using namespace literals;

/**
   This pass performs some simple semantic checks on the program;
   since the grammar accepts many programs that are actually illegal,
   this pass does some additional validation.

   This pass is run immediately after parsing. There is no type
   information at this point, so it only does simple checks.

   - integer constants have valid types
   - don't care _ is not used as a name for methods, fields, variables, instances
   - width of bit<> types is positive
   - width of int<> types is larger than 1
   - no parser state is named 'accept' or 'reject'
   - constructor parameters are direction-less
   - tables have an actions property
   - instantiations appear at the top-level only
   - the default label of a switch statement appears last
   - instantiations do not occur in actions
   - constructors are not invoked in actions
   - returns and exits do not appear in parsers
   - exits do not appear in functions
   - extern constructors have the same name as the enclosing extern
   - names of all parameters are distinct
   - no duplicate declarations in toplevel program
   - Dots are the last field
   - continue and break statements are only used in the context of a for statement
 */
class ValidateParsedProgram final : public Inspector {
    std::unordered_map<std::pair<const IR::Node *, cstring>, bool> annNames;

    void container(const IR::IContainer *type);
    // Make sure that type, apply and constructor parameters are distinct
    void distinctParameters(const IR::TypeParameters *typeParams, const IR::ParameterList *apply,
                            const IR::ParameterList *constr);

 public:
    ValidateParsedProgram() { setName("ValidateParsedProgram"); }
    void end_apply(const IR::Node *) override { annNames.clear(); }
    void postorder(const IR::Annotation *annotations) override;
    void postorder(const IR::P4Program *program) override;
    void postorder(const IR::Constant *c) override;
    void postorder(const IR::SwitchStatement *statement) override;
    void postorder(const IR::Method *t) override;
    void postorder(const IR::StructField *f) override;
    void postorder(const IR::ParserState *s) override;
    void postorder(const IR::P4Table *t) override;
    void postorder(const IR::Type_Bits *type) override;
    void postorder(const IR::Type_Varbits *type) override;
    void postorder(const IR::ConstructorCallExpression *expression) override;
    void postorder(const IR::Declaration_Variable *decl) override;
    void postorder(const IR::Declaration_Instance *inst) override;
    void postorder(const IR::Declaration_Constant *decl) override;
    void postorder(const IR::EntriesList *l) override;
    void postorder(const IR::ReturnStatement *statement) override;
    void postorder(const IR::ExitStatement *statement) override;
    void postorder(const IR::Type_Package *package) override { container(package); }
    void postorder(const IR::P4Control *control) override {
        container(control);
        distinctParameters(control->getTypeParameters(), control->getApplyParameters(),
                           control->getConstructorParameters());
    }
    void postorder(const IR::P4Parser *parser) override {
        auto start = parser->states.getDeclaration(IR::ParserState::start);
        if (!start) {
            error(ErrorType::ERR_INVALID, "Parser %1% has no 'start' state", parser->name);
        }
        container(parser);
        distinctParameters(parser->getTypeParameters(), parser->getApplyParameters(),
                           parser->getConstructorParameters());
    }
    void postorder(const IR::Dots *dots) override;
    void postorder(const IR::BreakStatement *s) override;
    void postorder(const IR::ContinueStatement *s) override;
};

}  // namespace P4

#endif /* FRONTENDS_P4_VALIDATEPARSEDPROGRAM_H_ */
