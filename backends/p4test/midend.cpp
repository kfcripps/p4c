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

#include "midend.h"

#include "frontends/common/constantFolding.h"
#include "frontends/common/resolveReferences/resolveReferences.h"
#include "frontends/p4-14/fromv1.0/v1model.h"
#include "frontends/p4/evaluator/evaluator.h"
#include "frontends/p4/moveDeclarations.h"
#include "frontends/p4/simplify.h"
#include "frontends/p4/simplifyParsers.h"
#include "frontends/p4/strengthReduction.h"
#include "frontends/p4/toP4/toP4.h"
#include "frontends/p4/typeChecking/typeChecker.h"
#include "frontends/p4/typeMap.h"
#include "frontends/p4/unusedDeclarations.h"
#include "ir/dump.h"
#include "midend/actionSynthesis.h"
#include "midend/checkTableEntries.h"
#include "midend/compileTimeOps.h"
#include "midend/complexComparison.h"
#include "midend/copyStructures.h"
#include "midend/def_use.h"
#include "midend/eliminateActionRun.h"
#include "midend/eliminateInvalidHeaders.h"
#include "midend/eliminateNewtype.h"
#include "midend/eliminateSerEnums.h"
#include "midend/eliminateSwitch.h"
#include "midend/eliminateTuples.h"
#include "midend/eliminateTypedefs.h"
#include "midend/expandEmit.h"
#include "midend/expandLookahead.h"
#include "midend/flattenHeaders.h"
#include "midend/flattenInterfaceStructs.h"
#include "midend/flattenLogMsg.h"
#include "midend/flattenUnions.h"
#include "midend/global_copyprop.h"
#include "midend/hsIndexSimplify.h"
#include "midend/local_copyprop.h"
#include "midend/midEndLast.h"
#include "midend/nestedStructs.h"
#include "midend/noMatch.h"
#include "midend/parserUnroll.h"
#include "midend/removeAssertAssume.h"
#include "midend/removeExits.h"
#include "midend/removeMiss.h"
#include "midend/removeSelectBooleans.h"
#include "midend/replaceSelectRange.h"
#include "midend/simplifyKey.h"
#include "midend/simplifySelectCases.h"
#include "midend/simplifySelectList.h"
#include "midend/tableHit.h"
#include "midend/unrollLoops.h"

namespace P4::P4Test {

class SkipControls : public P4::ActionSynthesisPolicy {
    const std::set<cstring> *skip;

 public:
    explicit SkipControls(const std::set<cstring> *skip) : skip(skip) { CHECK_NULL(skip); }
    bool convert(const Visitor::Context *, const IR::P4Control *control) override {
        if (skip->find(control->name) != skip->end()) return false;
        return true;
    }
};

MidEnd::MidEnd(P4TestOptions &options, std::ostream *outStream) {
    bool isv1 = options.langVersion == CompilerOptions::FrontendVersion::P4_14;
    refMap.setIsV1(isv1);
    auto evaluator = new P4::EvaluatorPass(&refMap, &typeMap);
    setName("MidEnd");

    auto v1controls = new std::set<cstring>();
    auto defUse = new P4::ComputeDefUse;

    addPasses(
        {new P4::DumpPipe("MidEnd start"),
         options.ndebug ? new P4::RemoveAssertAssume(&typeMap) : nullptr,
         new P4::RemoveMiss(&typeMap),
         new P4::EliminateNewtype(&typeMap),
         new P4::EliminateInvalidHeaders(&typeMap),
         new P4::EliminateSerEnums(&typeMap),
         new P4::SimplifyKey(
             &typeMap, new P4::OrPolicy(new P4::IsValid(&typeMap), new P4::IsLikeLeftValue())),
         new P4::CheckTableEntries(true),
         new P4::RemoveExits(&typeMap),
         new P4::ConstantFolding(&typeMap),
         new P4::SimplifySelectCases(&typeMap, false),  // non-constant keysets
         new P4::ExpandLookahead(&typeMap),
         new P4::ExpandEmit(&typeMap),
         new P4::HandleNoMatch(),
         new P4::SimplifyParsers(),
         new P4::StrengthReduction(&typeMap),
         new P4::EliminateTuples(&typeMap),
         new P4::FlattenLogMsg(&typeMap),
         new P4::SimplifyComparisons(&typeMap),
         new P4::CopyStructures(&typeMap, false),
         new P4::NestedStructs(&typeMap),
         new P4::StrengthReduction(&typeMap),
         new P4::SimplifySelectList(&typeMap),
         new P4::RemoveSelectBooleans(&typeMap),
         new P4::FlattenHeaders(&typeMap),
         new P4::FlattenInterfaceStructs(&typeMap),
         new P4::EliminateTypedef(&typeMap),
         new P4::ReplaceSelectRange(),
         new P4::MoveDeclarations(),  // more may have been introduced
         new P4::ConstantFolding(&typeMap),
         new P4::GlobalCopyPropagation(&refMap, &typeMap),
         new PassRepeated({
             new P4::LocalCopyPropagation(&typeMap),
             new P4::ConstantFolding(&typeMap),
         }),
         new P4::StrengthReduction(&typeMap),
         new P4::MoveDeclarations(),  // more may have been introduced
         new P4::SimplifyControlFlow(&typeMap, true),
         new P4::CompileTimeOperations(),
         new P4::TableHit(&typeMap),
         !options.preferSwitch ? new P4::EliminateSwitch(&typeMap) : nullptr,
         options.preferSwitch ? new P4::ElimActionRun() : nullptr,
         new P4::ResolveReferences(&refMap),
         new P4::TypeChecking(&refMap, &typeMap, true),  // update types before ComputeDefUse
         new PassRepeated({
             defUse,
             new P4::UnrollLoops(refMap, defUse),
             new P4::LocalCopyPropagation(&typeMap),
             new P4::ConstantFolding(&typeMap),
             new P4::StrengthReduction(&typeMap),
         }),
         new P4::MoveDeclarations(),  // more may have been introduced
         evaluator,
         [v1controls, evaluator](const IR::Node *root) -> const IR::Node * {
             const auto *toplevel = evaluator->getToplevelBlock();
             const auto *main = toplevel->getMain();
             if (main == nullptr) {
                 // nothing further to do.
                 return root;
             }
             // Special handling when compiling for v1model.p4
             if (main->type->name == P4V1::V1Model::instance.sw.name) {
                 if (main->getConstructorParameters()->size() != 6) return root;
                 auto verify = main->getParameterValue(P4V1::V1Model::instance.sw.verify.name);
                 auto update = main->getParameterValue(P4V1::V1Model::instance.sw.compute.name);
                 auto deparser = main->getParameterValue(P4V1::V1Model::instance.sw.deparser.name);
                 if (verify == nullptr || update == nullptr || deparser == nullptr ||
                     !verify->is<IR::ControlBlock>() || !update->is<IR::ControlBlock>() ||
                     !deparser->is<IR::ControlBlock>()) {
                     return root;
                 }
                 v1controls->emplace(verify->to<IR::ControlBlock>()->container->name);
                 v1controls->emplace(update->to<IR::ControlBlock>()->container->name);
                 v1controls->emplace(deparser->to<IR::ControlBlock>()->container->name);
             }
             return root;
         },
         new P4::HSIndexSimplifier(&typeMap),
         new P4::SynthesizeActions(&refMap, &typeMap, new SkipControls(v1controls)),
         new P4::MoveActionsToTables(&refMap, &typeMap),
         options.loopsUnrolling ? new P4::ParsersUnroll(true, &refMap, &typeMap) : nullptr,
         evaluator,
         [this, evaluator]() { toplevel = evaluator->getToplevelBlock(); },
         new P4::FlattenHeaderUnion(&refMap, &typeMap, options.loopsUnrolling),
         new P4::SimplifyControlFlow(&typeMap, true),
         new P4::MidEndLast(),
         new P4::DumpPipe("MidEnd end")});
    if (options.listMidendPasses) {
        listPasses(*outStream, cstring::newline);
        *outStream << std::endl;
        return;
    }
    if (options.excludeMidendPasses) {
        removePasses(options.passesToExcludeMidend);
    }
    addDebugHooks(hooks, true);
}

}  // namespace P4::P4Test
