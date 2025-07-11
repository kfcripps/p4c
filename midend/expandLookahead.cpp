/*
Copyright 2016 VMware, Inc.

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

#include "expandLookahead.h"

#include "frontends/p4/coreLibrary.h"
#include "frontends/p4/methodInstance.h"

namespace P4 {

void DoExpandLookahead::expand(
    const IR::PathExpression *bitvector,          // source value containing all bits
    const IR::Type *type,                         // type that is being extracted from source
    unsigned *offset,                             // current bit offset in source
    const IR::Expression *destination,            // result is assigned to this expression
    IR::IndexedVector<IR::StatOrDecl> *output) {  // add here new assignments
    if (type->is<IR::Type_Struct>() || type->is<IR::Type_Header>()) {
        if (type->is<IR::Type_Header>() && !expandHeader) return;
        if (type->is<IR::Type_Header>()) {
            auto setValid = new IR::Member(destination, IR::Type_Header::setValid);
            auto mc = new IR::MethodCallExpression(setValid);
            output->push_back(new IR::MethodCallStatement(mc));
        }
        auto st = type->to<IR::Type_StructLike>();
        for (auto f : st->fields) {
            auto t = typeMap->getTypeType(f->type, true);
            if (t == nullptr) continue;
            auto member = new IR::Member(destination, f->name);
            expand(bitvector, t, offset, member, output);
        }
    } else if (type->is<IR::Type_Bits>() || type->is<IR::Type_Boolean>()) {
        unsigned size = type->width_bits();
        if (size == 0) return;
        const IR::Expression *expression =
            new IR::Slice(bitvector->clone(), *offset - 1, *offset - size);
        auto tb = type->to<IR::Type_Bits>();
        if (!tb || tb->isSigned) expression = new IR::Cast(type, expression);
        *offset -= size;
        auto assignment = new IR::AssignmentStatement(bitvector->srcInfo, destination, expression);
        output->push_back(assignment);
    } else if (auto ts = type->to<IR::Type_Array>()) {
        unsigned elements = ts->getSize();
        auto etype = ts->elementType;
        for (unsigned i = 0; i < elements; i++) {
            auto member = new IR::ArrayIndex(destination, new IR::Constant(i));
            expand(bitvector, etype, offset, member, output);
        }
    } else {
        ::P4::error(ErrorType::ERR_UNEXPECTED, "%1%: unexpected type in lookahead argument", type);
    }
}

DoExpandLookahead::ExpansionInfo *DoExpandLookahead::convertLookahead(
    const IR::MethodCallExpression *expression) {
    if (expression == nullptr) return nullptr;
    auto mi = MethodInstance::resolve(expression, this, typeMap);
    if (!mi->is<P4::ExternMethod>()) return nullptr;
    auto em = mi->to<P4::ExternMethod>();
    if (em->originalExternType->name != P4CoreLibrary::instance().packetIn.name ||
        em->method->name != P4CoreLibrary::instance().packetIn.lookahead.name)
        return nullptr;

    // this is a call to packet_in.lookahead.
    BUG_CHECK(expression->typeArguments->size() == 1, "Expected 1 type parameter for %1%",
              em->method);
    auto targ = expression->typeArguments->at(0);
    auto typearg = typeMap->getTypeType(targ, true);
    if (!typearg->is<IR::Type_StructLike>() && !typearg->is<IR::Type_Tuple>()) return nullptr;

    if (typearg->is<IR::Type_Header>() && !expandHeader) return nullptr;

    int width = typeMap->widthBits(typearg, expression, false);
    if (width < 0) return nullptr;

    auto bittype = IR::Type_Bits::get(width);
    auto name = nameGen.newName("tmp");
    auto decl = new IR::Declaration_Variable(IR::ID(name), bittype, nullptr);
    newDecls.push_back(decl);

    auto ta = new IR::Vector<IR::Type>();
    ta->push_back(bittype);
    auto mc = new IR::MethodCallExpression(expression->srcInfo, expression->method->clone(), ta,
                                           expression->arguments);
    auto pathe = new IR::PathExpression(bittype, name);
    auto lookupCall = new IR::AssignmentStatement(expression->srcInfo, pathe, mc);
    auto result = new ExpansionInfo;
    result->statement = lookupCall;
    result->width = (unsigned)width;
    result->origType = typearg;
    result->tmp = pathe;
    return result;
}

const IR::Node *DoExpandLookahead::postorder(IR::MethodCallStatement *statement) {
    auto ei = convertLookahead(statement->methodCall);
    if (ei == nullptr) return statement;
    return ei->statement;
}

const IR::Node *DoExpandLookahead::postorder(IR::AssignmentStatement *statement) {
    if (!statement->right->is<IR::MethodCallExpression>()) return statement;

    auto ei = convertLookahead(statement->right->to<IR::MethodCallExpression>());
    if (ei == nullptr) return statement;
    auto result = new IR::BlockStatement;
    result->push_back(ei->statement);

    expand(ei->tmp->clone(), ei->origType, &ei->width, statement->left->clone(),
           &result->components);
    return result;
}

}  // namespace P4
