#ifndef BACKENDS_P4TOOLS_MODULES_TESTGEN_TARGETS_EBPF_TARGET_H_
#define BACKENDS_P4TOOLS_MODULES_TESTGEN_TARGETS_EBPF_TARGET_H_

#include <cstdint>
#include <filesystem>
#include <optional>

#include "backends/p4tools/common/lib/arch_spec.h"
#include "ir/ir.h"
#include "ir/solver.h"

#include "backends/p4tools/modules/testgen/core/program_info.h"
#include "backends/p4tools/modules/testgen/core/symbolic_executor/symbolic_executor.h"
#include "backends/p4tools/modules/testgen/core/target.h"
#include "backends/p4tools/modules/testgen/lib/execution_state.h"
#include "backends/p4tools/modules/testgen/targets/ebpf/cmd_stepper.h"
#include "backends/p4tools/modules/testgen/targets/ebpf/expr_stepper.h"
#include "backends/p4tools/modules/testgen/targets/ebpf/program_info.h"
#include "backends/p4tools/modules/testgen/targets/ebpf/test_backend.h"

namespace P4::P4Tools::P4Testgen::EBPF {

class EBPFTestgenTarget : public TestgenTarget {
 public:
    /// Registers this target.
    static void make();

 protected:
    const EBPFProgramInfo *produceProgramInfoImpl(
        const CompilerResult &compilerResult,
        const IR::Declaration_Instance *mainDecl) const override;

    EBPFTestBackend *getTestBackendImpl(const ProgramInfo &programInfo,
                                        const TestBackendConfiguration &testBackendConfiguration,
                                        SymbolicExecutor &symbex) const override;

    EBPFCmdStepper *getCmdStepperImpl(ExecutionState &state, AbstractSolver &solver,
                                      const ProgramInfo &programInfo) const override;

    EBPFExprStepper *getExprStepperImpl(ExecutionState &state, AbstractSolver &solver,
                                        const ProgramInfo &programInfo) const override;

 private:
    EBPFTestgenTarget();

    [[nodiscard]] MidEnd mkMidEnd(const CompilerOptions &options) const override;
};

}  // namespace P4::P4Tools::P4Testgen::EBPF

#endif /* BACKENDS_P4TOOLS_MODULES_TESTGEN_TARGETS_EBPF_TARGET_H_ */
