<!--!
\page changelog Releases                                                               
-->
<!-- 
Documentation Inclusion:
This README is integrated as a standalone page in the P4 compiler documentation.

Refer to the full page here: [Releases](https://p4lang.github.io/p4c/changelog.html)
-->
<!--!
\internal
-->
# Releases
<!--!
\endinternal
-->

## Semantic Versioning
We follow a monthly release cadence. Our versioning scheme is as follows:
- **Major.Minor.Patch** versions align with the P4 specification.
- **Revision** (the last number) is incremented with each monthly release.

> @note
> The commit history prior to the release [1.2.2.1](https://github.com/p4lang/p4c/pull/3085) is not included here but is available in the [commit history](https://github.com/p4lang/p4c/commits/main/).

## Release v1.2.5.3 [[view](https://github.com/p4lang/p4c/releases/tag/v1.2.5.3)]

### What's Changed 🎉

### Changes to the Compiler Core
- Switch to Abseil symbolization / stack tracing routines [[view](https://github.com/p4lang/p4c/pull/5077)] (Anton Korobeynikov).
- Add `P4::warningCount()` and `P4::infoCount()` [[view](https://github.com/p4lang/p4c/pull/5098)] (Vladimír Štill).
- Move all config handling into a single `config.h` file [[view](https://github.com/p4lang/p4c/pull/5102)] (Anton Korobeynikov).
- Fix duplication action name check and `LocalizeAllActions` [[view](https://github.com/p4lang/p4c/pull/4975)] (Andy Fingerhut).
- Fix `EnumeratorHandle` compilation [[view](https://github.com/p4lang/p4c/pull/5109)] (Bili Dong).

### Changes to the TC Back End
- Small fixes for P4TC [[view](https://github.com/p4lang/p4c/pull/5097)] (vbnogueira).

### Changes to the Tofino Back End
- Fix class reference in Tofino 2 driver [[view](https://github.com/p4lang/p4c/pull/5103)] (Fabian Ruffy).

### Other Changes
- Remove occurrences of "confidential" and "proprietary" [[view](https://github.com/p4lang/p4c/pull/5112)] (Andy Fingerhut).
- Update changelogs for v1.2.5.1 & v1.2.5.2 [[view](https://github.com/p4lang/p4c/pull/5117)] (Adarsh Rawat).
- Bump Doxygen v1.13.0 -> v1.13.2 [[view](https://github.com/p4lang/p4c/pull/5116)] (Adarsh Rawat).
- Automated Release v1.2.5.3 [[view](https://github.com/p4lang/p4c/pull/5118)] (GitHub Actions).

## Release v1.2.5.2 [[view](https://github.com/p4lang/p4c/releases/tag/v1.2.5.2)]

### Changes to the Compiler Core
- Fix `FlattenLogMsg` not properly working on nested structures [[view](https://github.com/p4lang/p4c/pull/5036)] (Jiri Havranek).
- Add checks to annotation getters [[view](https://github.com/p4lang/p4c/pull/5052)] (Fabian Ruffy).
- Mark RTTI methods as `const` / `pure` to enable compiler optimizations [[view](https://github.com/p4lang/p4c/pull/5049)] (Anton Korobeynikov).
- Ignore typeless structs in unstructured annotations [[view](https://github.com/p4lang/p4c/pull/5058)] (Chris Dodd).
- Teach function inliner to inline into `if` conditions [[view](https://github.com/p4lang/p4c/pull/5073)] (Anton Korobeynikov).

### Changes to the TC Back End
- Introduce the SKB metadata extern [[view](https://github.com/p4lang/p4c/pull/4916)] (vbnogueira).

### Changes to the DPDK Back End
- Added support for 128-bit constants usage in DPDK backend [[view](https://github.com/p4lang/p4c/pull/5074)] (Maheswari Subramanian).

### Changes to the Tofino Back End
- Fix incorrect annotation parsing in Tofino [[view](https://github.com/p4lang/p4c/pull/5051)] (Fabian Ruffy).
- Remove unnecessary file lists from Tofino CMake [[view](https://github.com/p4lang/p4c/pull/5039)] (Fabian Ruffy).
- Add a flag to enable `bf-asm` for Open P4Studio [[view](https://github.com/p4lang/p4c/pull/5056)] (Prathima Kotikalapudi).
- Clean up Tofino dynamic hash library [[view](https://github.com/p4lang/p4c/pull/5059)] (Fabian Ruffy).
- Fix missing indentation in `bfasm`-related commands [[view](https://github.com/p4lang/p4c/pull/5069)] (Fabian Ruffy).
- Remove `dynhash` library variable from Tofino library dependencies [[view](https://github.com/p4lang/p4c/pull/5072)] (Fabian Ruffy).
- Add a `bf-asm` command line option alongside environment variables [[view](https://github.com/p4lang/p4c/pull/5070)] (Fabian Ruffy).

### Other Changes
- Remove empty `v1.def` [[view](https://github.com/p4lang/p4c/pull/5029)] (Fabian Ruffy).
- Update documentation: Changelogs for v1.2.5.0 [[view](https://github.com/p4lang/p4c/pull/5045)] (Adarsh Rawat).
- Upgrade to Bazel 7.4.1 [[view](https://github.com/p4lang/p4c/pull/5071)] (John Cater).
- Remove obsolete implicit attribute `_cc_toolchain` [[view](https://github.com/p4lang/p4c/pull/5062)] (John Cater).
- Clean up `clang-format` and add a `.clangd` configuration [[view](https://github.com/p4lang/p4c/pull/5075)] (Fabian Ruffy).
- Remove unused test files from `testdata` [[view](https://github.com/p4lang/p4c/pull/5079)] (Andy Fingerhut).
- Bump Doxygen v1.12.0 -> v1.13.0 [[view](https://github.com/p4lang/p4c/pull/5080)] (Adarsh Rawat).
- Automated Release v1.2.5.2 [[view](https://github.com/p4lang/p4c/pull/5081)] (GitHub Actions).

## Release v1.2.5.1 [[view](https://github.com/p4lang/p4c/releases/tag/v1.2.5.1)]

### Breaking Changes 🛠
- Use type-safe discriminated union for `Annotation` [[view](https://github.com/p4lang/p4c/pull/5018)] (Anton Korobeynikov).

### Changes to the Compiler Core
- Remove some trivial memory leaks in codebase [[view](https://github.com/p4lang/p4c/pull/5012)] (Anton Korobeynikov).
- Fix `modifyAllMatching` visitor helper [[view](https://github.com/p4lang/p4c/pull/5020)] (Vladimír Štill).
- Inline some IR methods and constructors [[view](https://github.com/p4lang/p4c/pull/5030)] (Anton Korobeynikov).
- Remove unnecessary PIE workaround [[view](https://github.com/p4lang/p4c/pull/5044)] (Anton Korobeynikov).
- Add typed version of `isInContext()` and use it instead of `findContext()` [[view](https://github.com/p4lang/p4c/pull/5048)] (Anton Korobeynikov).

### Changes to the TC Back End
- Generate JSON actions and keys fields array even when empty [[view](https://github.com/p4lang/p4c/pull/5022)] (vbnogueira).

### Changes to the Tofino Back End
- Minor fix to Tofino CMake file [[view](https://github.com/p4lang/p4c/pull/5016)] (Han Wang).
- More Tofino include fixes [[view](https://github.com/p4lang/p4c/pull/5006)] (Fabian Ruffy).
- Fix multiple Tofino warnings [[view](https://github.com/p4lang/p4c/pull/5007)] (Fabian Ruffy).
- Port Tofino dynamic hash to C++ [[view](https://github.com/p4lang/p4c/pull/5043)] (Anton Korobeynikov).

### Other Changes
- Add Tofino to release workflow [[view](https://github.com/p4lang/p4c/pull/5017)] (Fabian Ruffy).
- Fix Mac dependency installation [[view](https://github.com/p4lang/p4c/pull/5028)] (Fabian Ruffy).
- Automated Release v1.2.5.1 [[view](https://github.com/p4lang/p4c/pull/5040)] (GitHub Actions).

## Release v1.2.5.0 [[view](https://github.com/p4lang/p4c/releases/tag/v1.2.5.1)]

### What's Changed 🎉
- The Tofino compiler is now open-source and available as a P4C back end 🎉 [[view](https://github.com/p4lang/p4c/pull/4964)] (Han Wang).

### Breaking Changes 🛠
- Add Abseil string helpers [[view](https://github.com/p4lang/p4c/pull/4971)] (Anton Korobeynikov).
- Remove `IR::Annotations` and make `IAnnotated` carry annotations inline [[view](https://github.com/p4lang/p4c/pull/4992)] (Anton Korobeynikov).

### P4 Specification Implementation
- Bump compiler version to 1.2.5.0 [[view](https://github.com/p4lang/p4c/pull/5005)] (Fabian Ruffy).

### Changes to the Compiler Core
- Fix broken Tofino build [[view](https://github.com/p4lang/p4c/pull/5004)] (Fabian Ruffy).

### Changes to the BMv2 Back Ends
- Enable handling of arbitrary `if` statements within actions for BMv2 back end [[view](https://github.com/p4lang/p4c/pull/4999)] (Andy Fingerhut).

### Changes to the TC Back End
- Small TC fixes [[view](https://github.com/p4lang/p4c/pull/5002)] (vbnogueira).

### Changes to the P4Tools Back End
- Replace `absl::btree_map` with the `P4::flat_map` implementation in P4Testgen [[view](https://github.com/p4lang/p4c/pull/4713)] (Fabian Ruffy).

### Changes to the Tofino Back End
- Miscellaneous small Tofino fixes for compilation [[view](https://github.com/p4lang/p4c/pull/4996)] (Fabian Ruffy).
- Remove superfluous Tofino includes and make include paths fully qualified [[view](https://github.com/p4lang/p4c/pull/4998)] (Fabian Ruffy).
- Fix Tofino backend issues due to recent annotations changes [[view](https://github.com/p4lang/p4c/pull/5015)] (Anton Korobeynikov).

### Other Changes
- Remove `CMAKE_BUILD_TYPE` values that do not work when building Z3 [[view](https://github.com/p4lang/p4c/pull/4997)] (Andy Fingerhut).
- Fix Clang compilation failures with `-fsized-deallocation` option [[view](https://github.com/p4lang/p4c/pull/4995)] (Fabian Ruffy).
- Update changelogs for v1.2.4.16 and v1.2.4.17 [[view](https://github.com/p4lang/p4c/pull/5003)] (Adarsh Rawat).

## Release v1.2.4.17 [[view](https://github.com/p4lang/p4c/pull/4990)]

### Breaking Changes 🛠
- Remove ReferenceMap from majority of midend passes [[view](https://github.com/p4lang/p4c/pull/4936)] (asl)
- Remove ReferenceMap from another set of midend passes [[view](https://github.com/p4lang/p4c/pull/4939)] (asl)

### P4 Specification Implementation
- Support for [lsb+:width] slices [[view](https://github.com/p4lang/p4c/pull/4917)] (Chris Dodd)

### Changes to the Compiler Core
- Fix parser symbolic interpreter to evaluate `StringLiteral` [[view](https://github.com/p4lang/p4c/pull/4937)] (jhavrane)
- Add `Vector::dbprint` [[view](https://github.com/p4lang/p4c/pull/4943)] (Vladimír Štill)
- Extend type checking to mark extern function call that returns enum as… [[view](https://github.com/p4lang/p4c/pull/4941)] (hanw)
- Reorder subdirectory additions in `CMakeLists.txt` [[view](https://github.com/p4lang/p4c/pull/4938)] (hanw)
- Add `LOGGING_FEATURE` macro [[view](https://github.com/p4lang/p4c/pull/4953)] (Vladimír Štill)
- Add utilities for running modifications inside nested IR nodes [[view](https://github.com/p4lang/p4c/pull/4940)] (Vladimír Štill)
- Set `PathExpression` type in `ExpandLookahead` [[view](https://github.com/p4lang/p4c/pull/4959)] (grg)
- Do not print confusing warning when a parser state contains an assignment to an l-value slice [[view](https://github.com/p4lang/p4c/pull/4948)] (kfcripps)
- Minor code/comments cleanup in `SimplifyDefUse` [[view](https://github.com/p4lang/p4c/pull/4963)] (kfcripps)
- Wrap `<64bit` types properly in loop unrolling [[view](https://github.com/p4lang/p4c/pull/4967)] (Chris Dodd)
- Fix warning about `delete` not matching `new` [[view](https://github.com/p4lang/p4c/pull/4989)] (Chris Dodd)

### Changes to the BMv2 Back Ends
- Clean up BMv2's `run-stf-test` script and integrate it with `testutils` [[view](https://github.com/p4lang/p4c/pull/4981)] (fruffy)

### Changes to the TC Back End
- Change `tc` template define [[view](https://github.com/p4lang/p4c/pull/4949)] (vbnogueira)
- Make number of masks default to 1 for exact tables [[view](https://github.com/p4lang/p4c/pull/4954)] (vbnogueira)
- Handle `pna_main_parser_input_metadata_t` fields in the TC backend's parser [[view](https://github.com/p4lang/p4c/pull/4955)] (vbnogueira)
- Implement Hash Extern for `P4TC` [[view](https://github.com/p4lang/p4c/pull/4980)] (komaljai)

### Changes to the DPDK Back End
- Support for 128-bit bitwise operation [[view](https://github.com/p4lang/p4c/pull/4952)] (Sosutha)

### Changes to the P4Tools Back End
- [P4Testgen] Move newline stripping from trace to `TestFramework` [[view](https://github.com/p4lang/p4c/pull/4946)] (Vladimír Štill)
- [P4Testgen] Run typechecking after front and mid end [[view](https://github.com/p4lang/p4c/pull/4834)] (fruffy)
- Build `Z3` from source instead of downloading precompiled binaries [[view](https://github.com/p4lang/p4c/pull/4697)] (fruffy)

### Other Changes
- Ignore `brew` dependency installation to fix breakages on MacOS CI [[view](https://github.com/p4lang/p4c/pull/4950)] (fruffy)
- Remove `ReferenceMap` from top-level frontend passes [[view](https://github.com/p4lang/p4c/pull/4947)] (asl)
- Add instructions on GitHub CI tests, including triggering optional ones [[view](https://github.com/p4lang/p4c/pull/4956)] (jafingerhut)
- Fix a recently introduced typo [[view](https://github.com/p4lang/p4c/pull/4960)] (jafingerhut)
- Fix Fedora CI build [[view](https://github.com/p4lang/p4c/pull/4957)] (fruffy)
- Add missing `ENABLE_*` flags to README [[view](https://github.com/p4lang/p4c/pull/4968)] (jafingerhut)
- Bump up `grpcio` version to fix Mac M1 CI issue [[view](https://github.com/p4lang/p4c/pull/4976)] (hanw)
- Require the driver binary as a test input [[view](https://github.com/p4lang/p4c/pull/4977)] (fruffy)
- [P4fmt]: Attach comments to IR Nodes [[view](https://github.com/p4lang/p4c/pull/4845)] (snapdgn)
- Contribute Intel Tofino compiler backend to `p4c` [[view](https://github.com/p4lang/p4c/pull/4964)] (hanw)
- Remove Tofino-specific Python packages from `requirements.txt` [[view](https://github.com/p4lang/p4c/pull/4984)] (fruffy)
- Fix Fedora build and CMake warnings for versions greater than CMake 3.24 [[view](https://github.com/p4lang/p4c/pull/4986)] (fruffy)
- Fix case of default CMake build mode [[view](https://github.com/p4lang/p4c/pull/4993)] (fruffy)

## Release v1.2.4.16 [[view](https://github.com/p4lang/p4c/pull/4935)]

### Breaking Changes 🛠
- Allow extending `ToP4`, clean its constructors [[view](https://github.com/p4lang/p4c/pull/4899)] (Vladimír Štill)

### Changes to the Compiler Core
- Reduce the number of memory allocations in `def-use` [[view](https://github.com/p4lang/p4c/pull/4904)] (asl)
- Split the `start` state more conservatively when a parser contains `decl` initializers [[view](https://github.com/p4lang/p4c/pull/4902)] (kfcripps)
- Add missed include [[view](https://github.com/p4lang/p4c/pull/4915)] (asl)
- Only process `IR::Path`s inside of `IR::ParserState` contexts in `MoveInitializers` [[view](https://github.com/p4lang/p4c/pull/4910)] (kfcripps)
- Factor common base `ExternCall` for `ExternMethod`/`ExternFunction` [[view](https://github.com/p4lang/p4c/pull/4898)] (Chris Dodd)
- [Issue - 4883](https://github.com/p4lang/p4c/issues/4883) Remove unused actions whose name starts with `"__"` [[view](https://github.com/p4lang/p4c/pull/4900)] (kfcripps)
- Allow `--Wdisable` to take precedence over `--Werror` for warning messages [[view](https://github.com/p4lang/p4c/pull/4894)] (kfcripps)
- Allow keywords to be used as annotation names [[view](https://github.com/p4lang/p4c/pull/4897)] (Chris Dodd)
- Avoid copying `out`/`inout` args when inlining functions [[view](https://github.com/p4lang/p4c/pull/4877)] (Chris Dodd)
- Clean up the `P4-14` dependent code [[view](https://github.com/p4lang/p4c/pull/4925)] (fruffy)
- Fix `Type_Indexed::at` [[view](https://github.com/p4lang/p4c/pull/4927)] (Chris Dodd)
- Make `--top4` matching case-insensitive [[view](https://github.com/p4lang/p4c/pull/4924)] (fruffy)

### Changes to the TC Back End
- Add `inst_type` field in `introspection.json` [[view](https://github.com/p4lang/p4c/pull/4905)] (komaljai)

### Changes to the P4Tools Back End
- [P4Tools] Track invocations in the timer to measure per function statistics [[view](https://github.com/p4lang/p4c/pull/4929)] (fruffy)
- [P4Testgen] Unify compiler and tool options; ensure options context is initialized correctly [[view](https://github.com/p4lang/p4c/pull/4787)] (fruffy)
- [P4Tools] Clean up use of `::P4` prefix for errors, warnings, and logs [[view](https://github.com/p4lang/p4c/pull/4930)] (fruffy)

### Other Changes
- Bump `peter-evans/create-pull-request` from 6 to 7 [[view](https://github.com/p4lang/p4c/pull/4903)] (dependabot)
- Docs: Update documentation instructions [[view](https://github.com/p4lang/p4c/pull/4875)] (AdarshRawat1)
- Remove workaround required for MacOS CI installation [[view](https://github.com/p4lang/p4c/pull/4921)] (fruffy)
- Docs: Changelog release update for v1.2.4.15 [[view](https://github.com/p4lang/p4c/pull/4934)] (AdarshRawat1)

## Release v1.2.4.15 [[view](https://github.com/p4lang/p4c/pull/4899)]

### Breaking Changes 🛠
- Fixup/cleanup `IR::Type::width_bits()` [[view](https://github.com/p4lang/p4c/pull/4858)] (Chris Dodd)
- Promote `P4` to be the project-wise top-level namespace [[view](https://github.com/p4lang/p4c/pull/4825)] (qobilidop)

### P4 Specification Implementation
- Allow concatenation of string literals at compile time [[view](https://github.com/p4lang/p4c/pull/4856)] (Vladimír Štill)

### Changes to the Compiler Core
- Ensure correct overload for `diagnose()` is called even in `Transform` context [[view](https://github.com/p4lang/p4c/pull/4830)] (asl)
- Fix ASAN issue in `lib/cstring` [[view](https://github.com/p4lang/p4c/pull/4865)] (Matthew Lam)
- Ensure proper `operator<<` is used during debug printing [[view](https://github.com/p4lang/p4c/pull/4860)] (asl)
- Fix linter complaints and enable `-werror` for MacOS [[view](https://github.com/p4lang/p4c/pull/4881)] (fruffy)
- Trim overlong source fragments in error messages [[view](https://github.com/p4lang/p4c/pull/4885)] (Chris Dodd)
- Pass to reduce added flags from `RemoveReturn` [[view](https://github.com/p4lang/p4c/pull/4878)] (Chris Dodd)
- Add `operator<` to some compiler classes [[view](https://github.com/p4lang/p4c/pull/4851)] (fruffy)
- Make `isSystemFile()` part of parser options file [[view](https://github.com/p4lang/p4c/pull/4888)] (fruffy)
- Add empty block for trailing case(s) with no code [[view](https://github.com/p4lang/p4c/pull/4889)] (Chris Dodd)
- Silence `-Wswitch-enum` from bison generated file [[view](https://github.com/p4lang/p4c/pull/4886)] (Chris Dodd)
- Sync `dump.h` with `dump.cpp` [[view](https://github.com/p4lang/p4c/pull/4896)] (qobilidop)
- Make type checking a true read-only visitor [[view](https://github.com/p4lang/p4c/pull/4829)] (asl)

### Changes to the Control Plane
- Set the initial default action for tables when generating a `P4Info` [[view](https://github.com/p4lang/p4c/pull/4773)] (fruffy)

### Changes to the BMv2 Back Ends
- Fix incorrect BMv2 include [[view](https://github.com/p4lang/p4c/pull/4843)] (fruffy)

### Changes to the TC Back End
- Support of `Digest` extern in P4TC [[view](https://github.com/p4lang/p4c/pull/4842)] (Komal Jain)
- Support for `is_net_port` and `is_host_port` externs [[view](https://github.com/p4lang/p4c/pull/4873)] (Komal Jain)
- Reduce extern parameters size to 64 bytes [[view](https://github.com/p4lang/p4c/pull/4880)] (vbnogueira)
- Remove priority field from ternary table's action [[view](https://github.com/p4lang/p4c/pull/4879)] (vbnogueira)
- Support for `Meter` and `DirectMeter` extern in P4TC [[view](https://github.com/p4lang/p4c/pull/4884)] (Komal Jain)

### Changes to the P4Tools Back End
- [P4Testgen] Represent bits of width zero as an empty string [[view](https://github.com/p4lang/p4c/pull/4852)] (fruffy)
- Reenable compilation tests for P4Smith [[view](https://github.com/p4lang/p4c/pull/4791)] (zzmic)
- [P4Tools] Clean up the P4Tools GTest infrastructure [[view](https://github.com/p4lang/p4c/pull/4841)] (fruffy)
- [P4Smith] Remove stray print in the smith code [[view](https://github.com/p4lang/p4c/pull/4891)] (fruffy)

### Other Changes
- Docs: Changelog release update for v1.2.4.14 [[view](https://github.com/p4lang/p4c/pull/4849)] (Adarsh Rawat)
- Feat: PR Preview Workflow via GitHub Pages [[view](https://github.com/p4lang/p4c/pull/4848)] (Adarsh Rawat)
- Fix: PR Preview URL to Use Base Repository [[view](https://github.com/p4lang/p4c/pull/4855)] (Adarsh Rawat)
- Bump Documentation build workflow action from Doxygen v1.11.0 to v1.12.0 [[view](https://github.com/p4lang/p4c/pull/4861)] (Adarsh Rawat)
- Bump actions/github-script from 6 to 7 [[view](https://github.com/p4lang/p4c/pull/4864)] (dependabot)
- Docs: Content Organization and Documentation Updates [[view](https://github.com/p4lang/p4c/pull/4850)] (Adarsh Rawat)
- [p4fmt]: Add a pretty printer for p4fmt [[view](https://github.com/p4lang/p4c/pull/4862)] (snapdgn)
- Docs:Content Organization & Add 'Frontend' and 'Midend' Sections [[view](https://github.com/p4lang/p4c/pull/4876)] (Adarsh Rawat)
- Update BDWGC [[view](https://github.com/p4lang/p4c/pull/4868)] (fruffy)
- Remove redundant tests [[view](https://github.com/p4lang/p4c/pull/4890)] (Andy Fingerhut)
-  Report specific uninitialized struct field in `simplifyDefUse` [[view](https://github.com/p4lang/p4c/pull/4892)] (Chris Dodd)
- Update namespace usage in `.gdbinit` [[view](https://github.com/p4lang/p4c/pull/4895)] (qobilidop)

## Release v1.2.4.14 [[view](https://github.com/p4lang/p4c/pull/4844)] 

### Breaking Changes 🛠
- P4C Options cleanup [[view](https://github.com/p4lang/p4c/pull/4790)] (Fabian Ruffy)
- Make preprocess and getIncludePath const functions. [[view](https://github.com/p4lang/p4c/pull/4785)] (Fabian Ruffy)

### P4 Specification Implementation
- Forbid case-after-default, tweak error messages [[view](https://github.com/p4lang/p4c/pull/4831)] (Vladimír Štill)
  
### Changes to the Compiler Core
- Improve cstring cache internals [[view](https://github.com/p4lang/p4c/pull/4780)] (Anton Korobeynikov)
- Fix loop unrolling bugs related to issue4739 [[view](https://github.com/p4lang/p4c/pull/4783)] (Chris Dodd)
- Make HasTableApply a resolution context, so it could resolve declarations on its own if desired [[view](https://github.com/p4lang/p4c/pull/4781)] (Anton Korobeynikov)
- [#4661] Do not unconditionally mark extern method calls as compile-time constants. [[view](https://github.com/p4lang/p4c/pull/4726)] (Kyle Cripps)
- Fix `loc_t::operator<` when `parent` is `nullptr` [[view](https://github.com/p4lang/p4c/pull/4798)] (Kyle Cripps)
- Introduce string map class and switch to it [[view](https://github.com/p4lang/p4c/pull/4774)] (Anton Korobeynikov)
- Require type decl in for..in loops [[view](https://github.com/p4lang/p4c/pull/4808)] (Chris Dodd)
- Add UnrollLoops::Policy to control unrolling per backend [[view](https://github.com/p4lang/p4c/pull/4809)] (Chris Dodd)
- Extend & fix FunctionInliner [[view](https://github.com/p4lang/p4c/pull/4801)] (Anton Korobeynikov)
- [NFC] Split TypeInference [[view](https://github.com/p4lang/p4c/pull/4814)] (Anton Korobeynikov)
- Map written `LocationSet`s to program locations (`loc_t`) instead of `IR::Expression*`s [[view](https://github.com/p4lang/p4c/pull/4797)] (Kyle Cripps)
- Store exact type, not one of the base class to inhibit assertion [[view](https://github.com/p4lang/p4c/pull/4818)] (Anton Korobeynikov)
- misc fixes/additions [[view](https://github.com/p4lang/p4c/pull/4820)] (Chris Dodd)
- Remove multimap usage from `memoizeDeclsByName` [[view](https://github.com/p4lang/p4c/pull/4821)] (Anton Korobeynikov)
- Make don't care args action-local when used in actions [[view](https://github.com/p4lang/p4c/pull/4817)] (Kyle Cripps)
- Add string literal support in constant folding. [[view](https://github.com/p4lang/p4c/pull/4837)] (Fabian Ruffy)
- Truncate shifts to the maximum constant bit-width in the constant folding pass. [[view](https://github.com/p4lang/p4c/pull/4836)] (Fabian Ruffy)   
- Make SymbolicVariable part of the core IR. [[view](https://github.com/p4lang/p4c/pull/4840)] (Fabian Ruffy)

### Changes to the BMv2 Back Ends
- Move ProgramStructure out the BMv2 folder such that it can be used in other back ends. [[view](https://github.com/p4lang/p4c/pull/4770)] (Fabian Ruffy)
- Add skeleton implementation for BMv2 PNA Backend [[view](https://github.com/p4lang/p4c/pull/4729)] (rupesh-chiluka-marvell)

### Changes to the TC Back End
- Support of InternetChecksum in P4TC [[view](https://github.com/p4lang/p4c/pull/4782)] (Komal Jain)
- Fix broken CI runs because missing override in TC code. [[view](https://github.com/p4lang/p4c/pull/4794)] (Fabian Ruffy)


### Changes to the P4Tools Back End

- P4Testgen logging cleanups. [[view](https://github.com/p4lang/p4c/pull/4684)] (Fabian Ruffy)
- For-loop and for-in-loop support for P4Smith [[view](https://github.com/p4lang/p4c/pull/4772)] (zzmic)
- [P4Testgen] Refactor the P4Testgen extern implementation. [[view](https://github.com/p4lang/p4c/pull/4728)] (Fabian Ruffy)
- [P4Testgen] Initialize the testgen targets when invoking the library API [[view](https://github.com/p4lang/p4c/pull/4706)] (Fabian Ruffy)
- [P4Testgen] Pass compiler options as argument instead of retrieving them from the context. [[view](https://github.com/p4lang/p4c/pull/4833)] (Fabian Ruffy) 
- [P4Testgen] Hotfix for failing P4Testgen benchmark test. [[view](https://github.com/p4lang/p4c/pull/4839)] (Fabian Ruffy)
- [P4Testgen] Fix problems with the reachability pass. [[view](https://github.com/p4lang/p4c/pull/4789)] (Fabian Ruffy)
- Replace boost::container::flat_map with absl::btree_map in P4Tools. [[view](https://github.com/p4lang/p4c/pull/4768)] (Fabian Ruffy)

### Other Changes
- Docs : Changelogs release update for v1.2.4.13 [[view](https://github.com/p4lang/p4c/pull/4788)] (Adarsh Rawat)
- Docs :  Add Home page for documentation  [[view](https://github.com/p4lang/p4c/pull/4764)] (Adarsh Rawat)
- P4Fmt reference checker [[view](https://github.com/p4lang/p4c/pull/4778)] (Nitish Kumar)
- Update the PINS programs. [[view](https://github.com/p4lang/p4c/pull/4799)] (Fabian Ruffy)
- Add test program to verify compile-time error if no type in for-in loop [[view](https://github.com/p4lang/p4c/pull/4812)] (Andy Fingerhut)
- Add tests from issue 4507 [[view](https://github.com/p4lang/p4c/pull/4816)] (Kyle Cripps)
- Update README.md instructions for running doxygen [[view](https://github.com/p4lang/p4c/pull/4807)] (Andy Fingerhut)
- Fix: Links in Generated Documentation [[view](https://github.com/p4lang/p4c/pull/4793)] (Adarsh Rawat)
- Minor Fix: links in P4Smith page [[view](https://github.com/p4lang/p4c/pull/4828)] (Adarsh Rawat)
- Add missing BUILD file dependencies. [[view](https://github.com/p4lang/p4c/pull/4846)] (Matthew Lam)
- Minor fix : Heading of portable_common [[view](https://github.com/p4lang/p4c/pull/4847)] (Adarsh Rawat)
- Automated Release v1.2.4.14 [[view](https://github.com/p4lang/p4c/pull/4844)]  (github-actions)

## Release v1.2.4.13 [[view](https://github.com/p4lang/p4c/pull/4767)]

### Breaking Changes 🛠
- Get rid of implicit char* => cstring conversions [[view](https://github.com/p4lang/p4c/pull/4694)].
- Format-related string fixes and refactorings [[view](https://github.com/p4lang/p4c/pull/4704)].
- Get rid of custom implementation of Utils::PathName [[view](https://github.com/p4lang/p4c/pull/4721)].
- cstring-related cleanup, switch to std::string_view for some cstring API [[view](https://github.com/p4lang/p4c/pull/4716)].
- Remove ReferenceMap recalculation (almost) everywhere and switch to more fine-grained solutions [[view](https://github.com/p4lang/p4c/pull/4757)].

### Changes to the Compiler Core
- Refactor error_helper and around [[view](https://github.com/p4lang/p4c/pull/4686)].
- Add freestanding RTTI::to, RTTI::is, RTTI::isAny [[view](https://github.com/p4lang/p4c/pull/4696)].
- More compilation fixes for C++20: hvec_map and copy_bitref [[view](https://github.com/p4lang/p4c/pull/4703)].
- Add a constant folding pass after inlining [[view](https://github.com/p4lang/p4c/pull/4727)].
- [#4006] Remove confusing warning when the same header is extracted multiple times in a parse path [[view](https://github.com/p4lang/p4c/pull/4725)].
- Emit true and false instead of 1 and 0 for BoolLiteral dbprint. [[view](https://github.com/p4lang/p4c/pull/4741)].
- [#4760] Create empty SetOfLocations for don't care arguments passed as action out args [[view](https://github.com/p4lang/p4c/pull/4762)].
- Delete unused RemoveComplexExpressions pass. [[view](https://github.com/p4lang/p4c/pull/4771)].

### Changes to the TC Back End
- P4TC - Support for DirectCounter extern [[view](https://github.com/p4lang/p4c/pull/4711)].
- Implementation of Counter extern in p4tc [[view](https://github.com/p4lang/p4c/pull/4734)].
- Make sure compiler_meta__->drop starts out false [[view](https://github.com/p4lang/p4c/pull/4722)].

### Changes to the DPDK Back End
- Update some stale DPDK error reference files. [[view](https://github.com/p4lang/p4c/pull/4705)].
- Fix a nullptr access in the DPDK back end. [[view](https://github.com/p4lang/p4c/pull/4712)].

### Changes to the P4Tools Back End
- Add P4Smith, a random program generator to the P4Tools framework [[view](https://github.com/p4lang/p4c/pull/4182)].
- [P4Smith] Fix broken links in Smith README [[view](https://github.com/p4lang/p4c/pull/4754)].

### Other Changes
- Configure: DOXYGEN for documentation generation [[view](https://github.com/p4lang/p4c/pull/4701)].
- Add a formatter binary skeleton as P4C back end [[view](https://github.com/p4lang/p4c/pull/4710)].
- Invalid include fix in p4fmt.cpp [[view](https://github.com/p4lang/p4c/pull/4718)].
- Test building P4C with ENABLE_GTESTS=OFF in CI [[view](https://github.com/p4lang/p4c/pull/4719)].
- Fix up the Bazel sed command. [[view](https://github.com/p4lang/p4c/pull/4720)].
- Docs : Configure Doxygen and GitHub pages deploy Action [[view](https://github.com/p4lang/p4c/pull/4702)].
- Fix : Compatibility issues with Doxygen version in runner and configuration [[view](https://github.com/p4lang/p4c/pull/4732)].
- Docs : Update community link from Slack to Zulip [[view](https://github.com/p4lang/p4c/pull/4735)].
- [Docs] Add initial CHANGELOG.md with changelogs from previous release [[view](https://github.com/p4lang/p4c/pull/4708)].
- Docs : Configuring Doxygen Awesome CSS [[view](https://github.com/p4lang/p4c/pull/4737)].
- Minor Fix: Graphs Backend Example Images [[view](https://github.com/p4lang/p4c/pull/4749)].
- Fix spurious P4Testgen failure occurring because of a problem when running PTF tests in parallel. [[view](https://github.com/p4lang/p4c/pull/4750)].
- Make different Markdown files have different titles in Doxygen output [[view](https://github.com/p4lang/p4c/pull/4745)].
- Bump docker/build-push-action from 5 to 6 [[view](https://github.com/p4lang/p4c/pull/4751)].
- Docs: Fixing diagram paths in generated docs [[view](https://github.com/p4lang/p4c/pull/4755)].
- Docs: Add theme dependency and instructions [[view](https://github.com/p4lang/p4c/pull/4747)].
- Add back in heading for P4Smith README file [[view](https://github.com/p4lang/p4c/pull/4756)].
- Docs: Initial sequence of pages in Documentation [[view](https://github.com/p4lang/p4c/pull/4758)].
- Docs: Update compiler file structure [[view](https://github.com/p4lang/p4c/pull/4761)].
- Fix naming of CMake Release configuration. [[view](https://github.com/p4lang/p4c/pull/4765)].
- Simplify dependabot label generation. [[view](https://github.com/p4lang/p4c/pull/4766)].
- Add DCO description to the contribution guidelines. [[view](https://github.com/p4lang/p4c/pull/4743)].
- Add a signature for the release bot for DCO support. [[view](https://github.com/p4lang/p4c/pull/4769)].
- Automated Release v1.2.4.13 [[view](https://github.com/p4lang/p4c/pull/4767)].

[Click here to find Full Changelog](https://github.com/p4lang/p4c/compare/v1.2.4.12...v1.2.4.13)

## Release v1.2.4.12 [[view](https://github.com/p4lang/p4c/pull/4699)]

### Breaking Changes 🛠
- Replace IR::getBitType with IR::Type_Bits::get. [[view](https://github.com/p4lang/p4c/pull/4669)]
- Make the new operator protected for some IR types. [[view](https://github.com/p4lang/p4c/pull/4670)]
  
### P4 Specification Implementation
- [#4656] Explicitly delay constant folding of only action enum `IR::SwitchCase` `label` expressions, instead of delaying constant folding of all `IR::Mux` expressions [[view](https://github.com/p4lang/p4c/pull/4657)]
- Changes for for loops [[view](https://github.com/p4lang/p4c/pull/4562)]

### Changes to the Compiler Core
- Use check_include_file_cxx instead of check_include_file to find mm_malloc.h [[view](https://github.com/p4lang/p4c/pull/4649)]
- Move IRUtils Literal get functions to the respective IR members. Add stringliteral get function.  [[view](https://github.com/p4lang/p4c/pull/4623)]
- Improve error message when shifting int by non-const [[view](https://github.com/p4lang/p4c/pull/4653)]
- Fixes to lib/hash and lib/big_int_util. [[view](https://github.com/p4lang/p4c/pull/4655)]
- Link against the Boehm-Demers-Weiser Garbage Collector using FetchContent. [[view](https://github.com/p4lang/p4c/pull/3930)]
- Explicitly include hash for Ubuntu 18.04. [[view](https://github.com/p4lang/p4c/pull/4664)]
- RemoveUnusedDeclarations - make helpers protected [[view](https://github.com/p4lang/p4c/pull/4668)]
- Generalization & minor refactoring in RenameMap [[view](https://github.com/p4lang/p4c/pull/4677)]
- Add string_view and string conversion operators and functions to cstring [[view](https://github.com/p4lang/p4c/pull/4676)]
- Facilitate inheritance from RenameSymbols pass, deduplicate code [[view](https://github.com/p4lang/p4c/pull/4682)]
- Set type for LAnd, LOr, LNot to be Type_Boolean if unknown. [[view](https://github.com/p4lang/p4c/pull/4612)]
- Improve `BUG_CHECK` internals [[view](https://github.com/p4lang/p4c/pull/4678)]
- Workaround for gcc-11.4/draft 2x spec flaw [[view](https://github.com/p4lang/p4c/pull/4679)]
- irgen: Generate explicit instantiations [[view](https://github.com/p4lang/p4c/pull/4681)]

### Changes to the Control Plane
- Add support for new platform property annotations for P4Runtime. [[view](https://github.com/p4lang/p4c/pull/4611)]

### Changes to the eBPF Back Ends
- Fix missing declaration of custom externs in the generated eBPF/uBPF header file. [[view](https://github.com/p4lang/p4c/pull/4644)]

### Changes to the TC Back End
- Support for Register Extern in P4TC [[view](https://github.com/p4lang/p4c/pull/4671)]
- Added changes for default hit actions for tc backend [[view](https://github.com/p4lang/p4c/pull/4673)]
- Fix extern pipeline name in template file [[view](https://github.com/p4lang/p4c/pull/4675)]

### Changes to the P4Tools Back End
- Merge TestgenCompilerTarget into TestgenTarget. [[view](https://github.com/p4lang/p4c/pull/4650)]

### Other Changes
- Change parameter for kfunc 'bpf_p4tc_entry_create_on_miss' [[view](https://github.com/p4lang/p4c/pull/4637)]
- Add support for a clang-tidy linter. Add a files utility function. [[view](https://github.com/p4lang/p4c/pull/4254)]
- Only emit the warning on EXPORT_COMPILE_COMMANDS when there are clang-tidy files to lint. [[view](https://github.com/p4lang/p4c/pull/4665)]
- Aggressively clean up the Protobuf CMake dependency. [[view](https://github.com/p4lang/p4c/pull/4543)]
- Remove no-longer-used *.p4info.txt files [[view](https://github.com/p4lang/p4c/pull/4687)]
- Add a release template to the compiler. [[view](https://github.com/p4lang/p4c/pull/4692)]

[Click here to find Full Changelog](https://github.com/p4lang/p4c/compare/v1.2.4.11...v1.2.4.12) 


## Release v1.2.4.11 [[view](https://github.com/p4lang/p4c/pull/4646)]

### Changes to the Compiler Core
- Introduce guard for aarch64 GCC compilation. [[view](https://github.com/p4lang/p4c/pull/4647)]
- Update MacOS brew version. [[view](https://github.com/p4lang/p4c/pull/4642)]
- Contribute DiagnosticCountInPassHook [[view](https://github.com/p4lang/p4c/pull/4629)]
- allow disabling of subtraction transform [[view](https://github.com/p4lang/p4c/pull/4633)]
- [#4625] Prune `MethodCallStatement`s if child `MethodCallExpression` resolves to a compile-time constant. [[view](https://github.com/p4lang/p4c/pull/4627)]
- [#4614] Unconditionally copy `dpdk` p4include files to the binary directory [[view](https://github.com/p4lang/p4c/pull/4615)]
- Call PassManager's debug hook even after a failing pass [[view](https://github.com/p4lang/p4c/pull/4626)]
- Ensure we take compiler-provided declaration of posix_memalign [[view](https://github.com/p4lang/p4c/pull/4609)]
- Fix missing override. [[view](https://github.com/p4lang/p4c/pull/4621)]
- Inspector: clear done/visit_in_progress when revisiting node [[view](https://github.com/p4lang/p4c/pull/4608)]
- Add option to build with dynamic libc and c++, but static other dependencies; change static option name [[view](https://github.com/p4lang/p4c/pull/4597)]
- Make the tree cloner also clone Member(TypeNameExpression)] to ensure the result is a DAG [[view](https://github.com/p4lang/p4c/pull/4539)]
- restore missing parentheses [[view](https://github.com/p4lang/p4c/pull/4600)]
- allow disabling of p4 input count check [[view](https://github.com/p4lang/p4c/pull/4446)]

### Changes to the eBPF Back Ends
- Do not swallow the test output when checking for the static binary functionality. [[view](https://github.com/p4lang/p4c/pull/4601)]

### Changes to the TC Back End
- Handle Arithmetic operations for fields which are in network order [[view](https://github.com/p4lang/p4c/pull/4566)]
- P4TC - Emit 'NoAction' in table actions list in template file [[view](https://github.com/p4lang/p4c/pull/4622)]
- Use `IR::Annotation::nameAnnotation` everywhere instead of hard-coding `@name` annotation name [[view](https://github.com/p4lang/p4c/pull/4628)]
- P4TC - Add permission annotations to tables  [[view](https://github.com/p4lang/p4c/pull/4610)]

### Changes to the P4Tools Back End
- Miscellaneous cleanups for P4Testgen. [[view](https://github.com/p4lang/p4c/pull/4613)]

### Other Changes 
- Add -mbmi flag to Abseil for GCC versions greater than 14.  [[view](https://github.com/p4lang/p4c/pull/4638)]
- Rename reused-counter.p4 as reused-counter-bmv2.p4 so a test is created for it [[view](https://github.com/p4lang/p4c/pull/4636)]
- Fix: Outdated links [[view](https://github.com/p4lang/p4c/pull/4618)]
- Update graphs comment formatting. [[view](https://github.com/p4lang/p4c/pull/4630)]
- [Docs]  capitalize "GCC" acronym in docs and comment [[view](https://github.com/p4lang/p4c/pull/4632)]
- Update ebpf comment format [[view](https://github.com/p4lang/p4c/pull/4624)]
- [Docs] Non-Breaking change of "p4c" to "P4C" [[view](https://github.com/p4lang/p4c/pull/4620)]
- Update the Commenting Style in the DPDK backend [[view](https://github.com/p4lang/p4c/pull/4619)]
- Force black to use pytoml as config. [[view](https://github.com/p4lang/p4c/pull/4588)]
- Format: Documentation dependency command [[view](https://github.com/p4lang/p4c/pull/4616)]
- Add missing <iterator> include to map.h [[view](https://github.com/p4lang/p4c/pull/4607)]
- Fix a warning emerging from a testsuite in GTest on newer versions of Clang.  [[view](https://github.com/p4lang/p4c/pull/4605)]
- Change kfunc definition for bpf_p4tc_tbl_read, xdp_p4tc_tbl_read, [[view](https://github.com/p4lang/p4c/pull/4604)]
- Update Commenting Style in Bmv2 Backend [[view](https://github.com/p4lang/p4c/pull/4603)]
- Update the dash programs. [[view](https://github.com/p4lang/p4c/pull/4593)]
- Update the comment format of common [[view](https://github.com/p4lang/p4c/pull/4602)]
- Create CONTRIBUTING.md [[view](https://github.com/p4lang/p4c/pull/4581)]
- [Docs] Add Table of Content [[view](https://github.com/p4lang/p4c/pull/4590)]
- [DOCS][Refactor] Replace direct links with word links [[view](https://github.com/p4lang/p4c/pull/4577)]
- Improve & Move "How to contribute" section to Top level readme  [[view](https://github.com/p4lang/p4c/pull/4580)]
- Fix spurious failure when two tests try to remove `ptf.log` at the same time. [[view](https://github.com/p4lang/p4c/pull/4591)]
- Revert using Abseil system includes. [[view](https://github.com/p4lang/p4c/pull/4594)]

## Release v1.2.4.10 [[view](https://github.com/p4lang/p4c/pull/4587)]
- [doc] backends: Add Doxygen-style documentation for BMV2 JSONObjects [[view](https://github.com/p4lang/p4c/pull/4554)]
- Add Comment Style Guide [[view](https://github.com/p4lang/p4c/pull/4573)]
- Fix rules_boost which pulls in the compromised xz dependency. [[view](https://github.com/p4lang/p4c/pull/4584)]
- [Fix] Resource links in Additional documentation [[view](https://github.com/p4lang/p4c/pull/4565)]
- Fix broken Debian CI test badge in the README. Add Fedora and MacOS CI badge.  [[view](https://github.com/p4lang/p4c/pull/4569)]
- Small build fixes. [[view](https://github.com/p4lang/p4c/pull/4564)]
- Added default action code for tc backend [[view](https://github.com/p4lang/p4c/pull/4561)]
- Add p4tc_filter_fields in add_entry [[view](https://github.com/p4lang/p4c/pull/4559)]
- Suppress Abseil warnings. [[view](https://github.com/p4lang/p4c/pull/4556)]
- Add a missing nullopt check. [[view](https://github.com/p4lang/p4c/pull/4560)]
- [P4Testgen] Clean up the direct extern map implementation for BMv2. [[view](https://github.com/p4lang/p4c/pull/4546)]
- Support new ActionProfile annotations for P4Runtime. [[view](https://github.com/p4lang/p4c/pull/4533)]
- Add width to parameters in const entries template definition. [[view](https://github.com/p4lang/p4c/pull/4557)]
- Implict cast fix [[view](https://github.com/p4lang/p4c/pull/4399)]
- Corrected version in README.md [[view](https://github.com/p4lang/p4c/pull/4555)]
- Refactor Util::Enumerator [[view](https://github.com/p4lang/p4c/pull/4513)]
- Add more CRC checksum variants to lib/nethash [[view](https://github.com/p4lang/p4c/pull/4550)]
- Bump black from 24.2.0 to 24.3.0 [[view](https://github.com/p4lang/p4c/pull/4551)]
- P4TC - Support add_on_miss, add entry externs [[view](https://github.com/p4lang/p4c/pull/4522)]
- Allow constructing an IR::ID from a std::string [[view](https://github.com/p4lang/p4c/pull/4549)]
- Move the network hashes implementation from Bmv2 testgen to lib/ [[view](https://github.com/p4lang/p4c/pull/4526)]
- Fix a warning on missing override in def use class. [[view](https://github.com/p4lang/p4c/pull/4547)]
- Add policy controlling RemoveUnusedDeclarations [[view](https://github.com/p4lang/p4c/pull/4528)]
- Use Abseil system includes [[view](https://github.com/p4lang/p4c/pull/4511)]
- Bump softprops/action-gh-release from 1 to 2 [[view](https://github.com/p4lang/p4c/pull/4545)]
- [documentation] Add hyperlinks to the actual directory in the README file [[view](https://github.com/p4lang/p4c/pull/4531)]
- Remove obsolete `output_to_genfiles = True`. [[view](https://github.com/p4lang/p4c/pull/4541)]
- [P4Testgen] Fix append and prepend invocation. [[view](https://github.com/p4lang/p4c/pull/4306)]
- Clean up P4Tools and control-plane linking. [[view](https://github.com/p4lang/p4c/pull/4520)]
- Midend def-use pass [[view](https://github.com/p4lang/p4c/pull/4489)]
- [docs] Sync Directory structure of P4tools  [[view](https://github.com/p4lang/p4c/pull/4516)]
- Preserve annotations on functions properly [[view](https://github.com/p4lang/p4c/pull/4532)]
- Add ninja as a default build system for some runners. [[view](https://github.com/p4lang/p4c/pull/4491)]
- Extend parser to allow parentheses in lvalues [[view](https://github.com/p4lang/p4c/pull/4530)]
- tc_may_override annotation implementation [[view](https://github.com/p4lang/p4c/pull/4529)]
- tc struct fields addition and memory initialization [[view](https://github.com/p4lang/p4c/pull/4524)]
- Allow annotations on functions [[view](https://github.com/p4lang/p4c/pull/4452)]
- Add 4 digit uid to dump file names [[view](https://github.com/p4lang/p4c/pull/4509)]
- help compiler folding for divideFloor/moduloFloor [[view](https://github.com/p4lang/p4c/pull/4512)]
- gc: implement posix_memalign. [[view](https://github.com/p4lang/p4c/pull/4508)]
- [docs] subdirectories and description for control plane [[view](https://github.com/p4lang/p4c/pull/4506)]
- Bump softprops/action-gh-release from 1 to 2 [[view](https://github.com/p4lang/p4c/pull/4518)]
 - YAML formatting from Redhead YAML language server. [[view](https://github.com/p4lang/p4c/pull/4521)]
- [Docs] Added tools to directory structure [[view](https://github.com/p4lang/p4c/pull/4504)]
 - [docs] Add Backend to directory structure [[view](https://github.com/p4lang/p4c/pull/4503)]
- Fix broken MacOS and Ubuntu18.04 runs.  [[view](https://github.com/p4lang/p4c/pull/4517)]
- Update formatters (clang-format, isort, black)] [[view](https://github.com/p4lang/p4c/pull/4515)]
- HalfOpenRange/ClosedRange classes [[view](https://github.com/p4lang/p4c/pull/4496)]
- Link bazel badge to the bazel build ci [[view](https://github.com/p4lang/p4c/pull/4505)]
- Build P4Testgen on MacOS. [[view](https://github.com/p4lang/p4c/pull/4492)]
- Replace boost string algorithms with their abseil counterparts [[view](https://github.com/p4lang/p4c/pull/4482)]
- Move CI Mac and Fedora tests to separate test runs. [[view](https://github.com/p4lang/p4c/pull/4495)]
- Open up the Z3 api. [[view](https://github.com/p4lang/p4c/pull/4322)]
- fix comment typo [[view](https://github.com/p4lang/p4c/pull/4498)]
- restore underscore after DISABLED in test name [[view](https://github.com/p4lang/p4c/pull/4497)]
- warn on unused controls/parsers [[view](https://github.com/p4lang/p4c/pull/4440)]
- [P4Testgen] Add a rudimentary P4Testgen benchmark with via GTest. [[view](https://github.com/p4lang/p4c/pull/4475)]

## Release v1.2.4.9 [[view](https://github.com/p4lang/p4c/pull/4490)]
- Move out of place comment in gc.cpp [[view](https://github.com/p4lang/p4c/pull/4494)]
- Use abseil maps even more [[view](https://github.com/p4lang/p4c/pull/4473)]
- Fetch latest brew formulae [[view](https://github.com/p4lang/p4c/pull/4488)]
- Generalize get in map utils [[view](https://github.com/p4lang/p4c/pull/4483)]
- Distinguish runner OS. [[view](https://github.com/p4lang/p4c/pull/4487)]
- Fixes from static analysis [[view](https://github.com/p4lang/p4c/pull/4442)]
- Add missing Bazel targets. [[view](https://github.com/p4lang/p4c/pull/4486)]
- Update MacOS runner to Ventura, add MacOS Sonoma (M1)] runner [[view](https://github.com/p4lang/p4c/pull/4393)]
- [P4Testgen] Add an option to selectively ignore control plane elements.  [[view](https://github.com/p4lang/p4c/pull/4417)]
- backends/tc: Fix issues reported in #4327 [[view](https://github.com/p4lang/p4c/pull/4484)]
- Fix for default_action [[view](https://github.com/p4lang/p4c/pull/4485)]
+ Removed pipeline id from template for tc backend [[view](https://github.com/p4lang/p4c/pull/4480)]
- Try to clean up the Protobuf includes. [[view](https://github.com/p4lang/p4c/pull/4474)]
- Use better maps to store visitor state [[view](https://github.com/p4lang/p4c/pull/4459)]
- [P4Testgen] Implement a library for common control-plane symbolic variables. [[view](https://github.com/p4lang/p4c/pull/4398)]
- Dowload pre-built Z3 on Macs as well [[view](https://github.com/p4lang/p4c/pull/4476)]
- Bump protobuf version and add Abseil as compiler dependency.  [[view](https://github.com/p4lang/p4c/pull/4463)]
- Try to speed up the CI build process [[view](https://github.com/p4lang/p4c/pull/4470)]
- Add some more global operator new / delete overrides [[view](https://github.com/p4lang/p4c/pull/4465)]
- Add support for parser errors in tc backend [[view](https://github.com/p4lang/p4c/pull/4443)]
- Bump docker/build-push-action from 3 to 5 [[view](https://github.com/p4lang/p4c/pull/4467)]
- Bump actions/setup-python from 4 to 5 [[view](https://github.com/p4lang/p4c/pull/4466)]
- Fix hvec_map insert/emplace [[view](https://github.com/p4lang/p4c/pull/4458)]
- Remove some unused boost headers and switch from boost code to alternatives [[view](https://github.com/p4lang/p4c/pull/4464)]
- Handle value suffixes in the asserts parser. [[view](https://github.com/p4lang/p4c/pull/4450)]
- Add more API options to the P4Testgen api. [[view](https://github.com/p4lang/p4c/pull/4451)]
- Add merge group label to relevant branches. [[view](https://github.com/p4lang/p4c/pull/4462)]
- Refactor ReferenceResolver to use native C++-enumerators in some places [[view](https://github.com/p4lang/p4c/pull/4432)]
- Bump actions/checkout from 3 to 4 [[view](https://github.com/p4lang/p4c/pull/4457)]
- Bump actions/cache from 2 to 4 [[view](https://github.com/p4lang/p4c/pull/4456)]
- Bump docker/setup-buildx-action from 2 to 3 [[view](https://github.com/p4lang/p4c/pull/4455)]
- Bump peter-evans/create-pull-request from 5 to 6 [[view](https://github.com/p4lang/p4c/pull/4454)]
- Bump docker/login-action from 2 to 3 [[view](https://github.com/p4lang/p4c/pull/4453)]
- Refactor some visitor internals [[view](https://github.com/p4lang/p4c/pull/4447)]
- Create a dependabot.yml for Github actions [[view](https://github.com/p4lang/p4c/pull/4405)]
- Fix comment [[view](https://github.com/p4lang/p4c/pull/4449)]
- Change minimum key struct alignment to 8 [[view](https://github.com/p4lang/p4c/pull/4426)]
- P4TC - CRC32 Initialisation [[view](https://github.com/p4lang/p4c/pull/4427)]
- Avoid to-after-is double calls to RTTI in type checker [[view](https://github.com/p4lang/p4c/pull/4441)]
- add missing srcInfo to ParserState [[view](https://github.com/p4lang/p4c/pull/4438)]
- Move information logging to toplevel. [[view](https://github.com/p4lang/p4c/pull/4436)]
- Clean up Python linters. [[view](https://github.com/p4lang/p4c/pull/4437)]
- -lldb arg run run lldb on the compiler in tests [[view](https://github.com/p4lang/p4c/pull/4435)]
- Don't apply defaultArguments in table action lists [[view](https://github.com/p4lang/p4c/pull/4434)]
- Error casting bool const to other than bit<1> [[view](https://github.com/p4lang/p4c/pull/4419)]
- Provide better hash functions and boilerplate [[view](https://github.com/p4lang/p4c/pull/4424)]
- allow json output format to be modified [[view](https://github.com/p4lang/p4c/pull/4407)]
- Pass FrontEndPolicy to unit tests [[view](https://github.com/p4lang/p4c/pull/4433)]
- Basic support for `@p4runtime_translation` and `@p4runtime_translation_mappings`. [[view](https://github.com/p4lang/p4c/pull/4363)]
- Add a missing build rule to Bazel. [[view](https://github.com/p4lang/p4c/pull/4429)]
- [P4Testgen] Add an option to only generate tests with dropped packets.  [[view](https://github.com/p4lang/p4c/pull/4416)]
- [P4Testgen] Look up the alias in the P4Info instead of using the fully qualified control plane name for Protobuf IR tests. [[view](https://github.com/p4lang/p4c/pull/4425)]
- frontend policy fix: missed ConstantFolding [[view](https://github.com/p4lang/p4c/pull/4423)]
- gtest include: quotes -> angled brackets [[view](https://github.com/p4lang/p4c/pull/4422)]
- test for ContantFolding + policy [[view](https://github.com/p4lang/p4c/pull/4421)]
- Add IR::inlineBlock utility that helps with return of multiple statements from Transform [[view](https://github.com/p4lang/p4c/pull/4414)]
- Emit a reasonable error if someone uses type in place where expression is exprected [[view](https://github.com/p4lang/p4c/pull/4411)]
- Introduce a frontend policy as a customization point for frontend [[view](https://github.com/p4lang/p4c/pull/4406)]
- Add a P4Testgen library API. [[view](https://github.com/p4lang/p4c/pull/4374)]
- Implement lightweight RTTI for (not only)] IR::Node class hierarchies [[view](https://github.com/p4lang/p4c/pull/4377)]
- Add buildifier to Bazel workspace and run it on the bazel build files. [[view](https://github.com/p4lang/p4c/pull/4413)]
- Memoize & improve name lookup in `ResolveReferences`  [[view](https://github.com/p4lang/p4c/pull/4376)]
- Stop when an error is throwing in the P4Runtime serializer. Make stop on error explicit in pass manager. [[view](https://github.com/p4lang/p4c/pull/4408)]
- Do not check for shadowing repeatedly in inlining passes. [[view](https://github.com/p4lang/p4c/pull/4396)]
- Add an option to override the test name. [[view](https://github.com/p4lang/p4c/pull/4401)]
- Add a silent mode to P4Tools. Add performance counting at the top level. [[view](https://github.com/p4lang/p4c/pull/4369)]
- Introduce a configuration structure for the test back ends. [[view](https://github.com/p4lang/p4c/pull/4372)]
- Implement copy assignment operator for hvec_map [[view](https://github.com/p4lang/p4c/pull/4388)]
- [P4Testgen] Fix generation of specific fields in the Protobuf IR back end. Move hex separator utility to common class. [[view](https://github.com/p4lang/p4c/pull/4400)]
- Improve diagnostics for `MethodCallExpression` [[view](https://github.com/p4lang/p4c/pull/4354)]
- Step 4: Use the P4Runtime API in the Protobuf back end. [[view](https://github.com/p4lang/p4c/pull/4303)]
- Fix failing P4Testgen gtest. [[view](https://github.com/p4lang/p4c/pull/4397)]
- Dpdk backend:Explicitly use method toString()] while moving isValid()] Methodcall used as table key, to metadata. [[view](https://github.com/p4lang/p4c/pull/4383)]
- [#4365] Do not allow `--Wdisable` or `--Wwarn` to demote errors. Allow `--Winfo=diagnostic` to work for `diagnostic`s that can be both warnings and errors. [[view](https://github.com/p4lang/p4c/pull/4366)]
- Rename eBPF section names for tc backend [[view](https://github.com/p4lang/p4c/pull/4361)]
- Fixes from static analysis [[view](https://github.com/p4lang/p4c/pull/4391)]
- [P4Testgen] Hotfixes and improvements to the P4Constraints parsers. [[view](https://github.com/p4lang/p4c/pull/4387)]
- backends/ebpf: Track header offset in bytes rather than bits. [[view](https://github.com/p4lang/p4c/pull/4327)]
- backends/tc: Honour P4TEST_REPLACE environment variable like other backends. [[view](https://github.com/p4lang/p4c/pull/4395)]
- Use proper Github actions syntax. [[view](https://github.com/p4lang/p4c/pull/4394)]
- Allow one more round of type inference after specialization [[view](https://github.com/p4lang/p4c/pull/4291)]

## Release v1.2.4.8 [[view](https://github.com/p4lang/p4c/pull/4386)]
- Add a P4Info API to the control-plane folder and P4Tools. [[view](https://github.com/p4lang/p4c/pull/4381)]
- [NFC] Eliminate the majority of `dynamic_cast` usage in the codebase in favor of ICastable interface [[view](https://github.com/p4lang/p4c/pull/4382)]
- Tweak optional CI run execution. [[view](https://github.com/p4lang/p4c/pull/4380)]
- Remove unused parser declarations and unused parser type declarations that result from `RemoveRedundantParsers` pass. [[view](https://github.com/p4lang/p4c/pull/4368)]
- Remove redundant protobuf installation. [[view](https://github.com/p4lang/p4c/pull/4379)]
- Remove XDP code generated for TC [[view](https://github.com/p4lang/p4c/pull/4349)]
- Use std::allocator_traits::rebind_alloc instead of std::allocator::rebind [[view](https://github.com/p4lang/p4c/pull/4373)]
- Support ebpf Hash and Checksum [[view](https://github.com/p4lang/p4c/pull/4378)]
- P4TC - Support const entries [[view](https://github.com/p4lang/p4c/pull/4329)]
- Fixes for MacOS/macports. [[view](https://github.com/p4lang/p4c/pull/4375)]
- Fix compiler stdin. [[view](https://github.com/p4lang/p4c/pull/4367)]
- Add missing equiv implementation to Type_Any. [[view](https://github.com/p4lang/p4c/pull/4336)]
- Remove options from gtest binary. [[view](https://github.com/p4lang/p4c/pull/4334)]
- Provide hints on how to fix CI complaints. [[view](https://github.com/p4lang/p4c/pull/4355)]
- Improve diagnostics for AssignmentStatement [[view](https://github.com/p4lang/p4c/pull/4360)]
- Improve diagnostics for `BaseListExpression` [[view](https://github.com/p4lang/p4c/pull/4358)]
- Improve diagnostics for `StructExpression` [[view](https://github.com/p4lang/p4c/pull/4357)]
- [P4Testgen] Introduce a compiler target for P4Testgen. Move computation from the ProgramInfo to the midend. [[view](https://github.com/p4lang/p4c/pull/4292)]
 - Deprecate .txt, support in favour of .txtpb.  [[view](https://github.com/p4lang/p4c/pull/4352)]
- [P4Testgen] Introduce the option to produce lowercase hex for the formatting library. [[view](https://github.com/p4lang/p4c/pull/4340)]
- Trigger CI workflows based on assigned label. [[view](https://github.com/p4lang/p4c/pull/4348)]
- Emit the Protobuf header with the P4 entries and runtime files. [[view](https://github.com/p4lang/p4c/pull/4350)]
- DoConstantFolding: make typeMap param const [[view](https://github.com/p4lang/p4c/pull/4346)]
- Repair the static DPDK PTF tests. [[view](https://github.com/p4lang/p4c/pull/4210)]
- Documentation & testfix follow-up to #4160 [[view](https://github.com/p4lang/p4c/pull/4328)]
- cstring: Add string literal suffix, add toLower [[view](https://github.com/p4lang/p4c/pull/4342)]
- Allow unknown fields when parsing P4Info files [[view](https://github.com/p4lang/p4c/pull/4341)]
- Convert deparser header to 'inout' and address casting for functions with control block parameters  [[view](https://github.com/p4lang/p4c/pull/4338)]
- Step 2: Refactor ProgramInfo to be initialized with CompilerResult instead of a IR::P4Program. [[view](https://github.com/p4lang/p4c/pull/4324)]
- Ignore clang-tidy complaints about macro do-while loops. [[view](https://github.com/p4lang/p4c/pull/4332)]
- Fix build with system Protobuf [[view](https://github.com/p4lang/p4c/pull/4321)]
- [P4Testgen] Extend the CompilerTarget runProgram function with data structures which can pass on more information. [[view](https://github.com/p4lang/p4c/pull/4323)]
- Update status badges [[view](https://github.com/p4lang/p4c/pull/4330)]
- Allow constant-folding of arbitrary-precision integer casts [[view](https://github.com/p4lang/p4c/pull/4325)]
- Rename --with-output-packet to --output-packet-only. [[view](https://github.com/p4lang/p4c/pull/4314)]
- Make sure P4 expression optimization does not strip away types [[view](https://github.com/p4lang/p4c/pull/4300)]
- Respect ENABLE_LTO even for static build, but keep LTO default for static [[view](https://github.com/p4lang/p4c/pull/4320)]
- Allow constructing PassRepeated from a PassManager [[view](https://github.com/p4lang/p4c/pull/4319)]
- keep P4Control->body->srcInfo during init move [[view](https://github.com/p4lang/p4c/pull/4317)]
- [P4Testgen] Implement coverage tracking of actions [[view](https://github.com/p4lang/p4c/pull/4307)]

## Release v1.2.4.7 [[view](https://github.com/p4lang/p4c/pull/4312)]
- Fix some problems with the Protobuf CMake file [[view](https://github.com/p4lang/p4c/pull/4262)]
- Fix actionparams type field in introspection.json [[view](https://github.com/p4lang/p4c/pull/4313)]
- Move arch spec to ProgramInfo to allow it to depend on P4 program [[view](https://github.com/p4lang/p4c/pull/4267)]
- Support default action in P4TC [[view](https://github.com/p4lang/p4c/pull/4308)]
- [P4Testgen] Add support for @format annotations in P4 programs for protobuf-ir tests [[view](https://github.com/p4lang/p4c/pull/4276)]
- Remove -static and rename static build appropriately. [[view](https://github.com/p4lang/p4c/pull/4284)]
- Fix incorrect field extraction arithmetic when multiple non-byte aligned header fields are combined into single byte aligned field [[view](https://github.com/p4lang/p4c/pull/4301)]
- [P4Testgen] Fix stringliteral conversion. Value must be a literal, NOT a constant [[view](https://github.com/p4lang/p4c/pull/4298)]
- Remove uses of `incompatible_use_toolchain_transition`. [[view](https://github.com/p4lang/p4c/pull/4299)]
- Fix initialization order in gtest helper [[view](https://github.com/p4lang/p4c/pull/4296)]
- [P4Testgen] Fix behavior of coverage on edge cases with no nodes to cover [[view](https://github.com/p4lang/p4c/pull/4275)]
- Add validation of constant enum values fitting [[view](https://github.com/p4lang/p4c/pull/4287)]
- Do not toggle BOOST_NO_ARGUMENT_DEPENDENT_LOOKUP in CMakelists.txt [[view](https://github.com/p4lang/p4c/pull/4181)]
- P4TC - Support for set_entry_timer extern [[view](https://github.com/p4lang/p4c/pull/4269)]
- Try to fix some issues with slowness in the MacOS build. [[view](https://github.com/p4lang/p4c/pull/4280)]
- Move P4Testgen's logging function into the common library. [[view](https://github.com/p4lang/p4c/pull/4279)]
- Remove deprecated inheritance. [[view](https://github.com/p4lang/p4c/pull/4283)]
- [P4Testgen] Fix accidental system file categorization. [[view](https://github.com/p4lang/p4c/pull/4278)]
- Fix problem with bpftool in PTF tests [[view](https://github.com/p4lang/p4c/pull/4277)]
- [P4Testgen] Move static helpers from TestgenTarget and ProgramInfo to common/lib/utils [[view](https://github.com/p4lang/p4c/pull/4268)]
- [P4Testgen] Resolve method call arguments before stepping into an extern - preserve InOut references [[view](https://github.com/p4lang/p4c/pull/4255)]

## Release v1.2.4.6 [[view](https://github.com/p4lang/p4c/pull/4271)]
- Remove stale XFail. [[view](https://github.com/p4lang/p4c/pull/4270)]
- [P4Testgen] Introduce a new Protobuf backend which uses P4 PDPI instead of P4Runtime  [[view](https://github.com/p4lang/p4c/pull/4221)]
- [P4Testgen]  Add struct and header expressions as a first-class expression to P4Testgen.  [[view](https://github.com/p4lang/p4c/pull/4231)]
- P4tc Runtime issue fix [[view](https://github.com/p4lang/p4c/pull/4263)]
- Fix clang-tidy config file. [[view](https://github.com/p4lang/p4c/pull/4253)]
- More code improvements based on static analysis [[view](https://github.com/p4lang/p4c/pull/4258)]
- [P4Testgen] Flatten the genEq function and support struct expressions. [[view](https://github.com/p4lang/p4c/pull/4225)]
- [P4Testgen] Add a compiler pass to resolve Type_Name in StructExpressions. [[view](https://github.com/p4lang/p4c/pull/4215)]
- Rename TF to TestFramework [[view](https://github.com/p4lang/p4c/pull/4247)]
- [P4Testgen] Generate NoMatch for selects without default [[view](https://github.com/p4lang/p4c/pull/4250)]
- [P4Testgen] Fix argument names in GenericDescription trace event [[view](https://github.com/p4lang/p4c/pull/4257)]
- [P4Testgen] Add --assert-min-coverage option to check coverage of generated test cases [[view](https://github.com/p4lang/p4c/pull/4251)]
- [P4Testgen] Change crash message [[view](https://github.com/p4lang/p4c/pull/4252)]
- Added range check of error codes [[view](https://github.com/p4lang/p4c/pull/4249)]
- Fixed ebpf_packetOffsetInBits field in parser and control_block C file [[view](https://github.com/p4lang/p4c/pull/4233)]
- Ensure accurate typing for functions that work with references. [[view](https://github.com/p4lang/p4c/pull/4223)]
- Allow non-cast checkedTo cast. [[view](https://github.com/p4lang/p4c/pull/4243)]
- Allow custom usage in backends [[view](https://github.com/p4lang/p4c/pull/4244)]
- Fix some problems and suggestions found by clang-tidy [[view](https://github.com/p4lang/p4c/pull/4237)]
- [P4Testgen] Add EliminateInvalidHeaders midend pass [[view](https://github.com/p4lang/p4c/pull/4239)]
- Report offending node in the type-checker immutability check [[view](https://github.com/p4lang/p4c/pull/4238)]
- Allow method calls in index expressions in hsIndexSimplify [[view](https://github.com/p4lang/p4c/pull/4240)]
- Simplify jsonAssignment in ParserConverter and ActionConverter [[view](https://github.com/p4lang/p4c/pull/4232)]
- Fix build for latest Fedora version [[view](https://github.com/p4lang/p4c/pull/4241)]
- Disable DPDK PTF tests. Remove the deprecated travis.yml file. [[view](https://github.com/p4lang/p4c/pull/4228)]
- improved bootstrap.sh. [[view](https://github.com/p4lang/p4c/pull/4227)]
- [P4Testgen] Move common code of BMv2 test back ends into a common class. [[view](https://github.com/p4lang/p4c/pull/4220)]
- Flatten nesting of test backends and rename the folder appropriately. [[view](https://github.com/p4lang/p4c/pull/4219)]
- #noconstructor should also include the json loader constructor [[view](https://github.com/p4lang/p4c/pull/4216)]
- Fix some issues found by static analysis [[view](https://github.com/p4lang/p4c/pull/4213)]
- Ignore flaky test in P4Testgen eBPF back end. [[view](https://github.com/p4lang/p4c/pull/4224)]
- Still parse STF files, even when BMv2 model is not present in system. [[view](https://github.com/p4lang/p4c/pull/4222)]

## Release v1.2.4.5 [[view](https://github.com/p4lang/p4c/pull/4217)]
- Benchmarking fixes for P4Testgen. [[view](https://github.com/p4lang/p4c/pull/4205)]
- Add a testgen.p4 file with custom extern definitions such as testgen_assert and testgen_assume. [[view](https://github.com/p4lang/p4c/pull/4214)]
- Fix textproto generation and escape traces properly. [[view](https://github.com/p4lang/p4c/pull/4207)]
- Fix P4TC issues [[view](https://github.com/p4lang/p4c/pull/4209)]
- generic -O option to set optimization level/options [[view](https://github.com/p4lang/p4c/pull/4206)]
- Fix another segmentation fault when using the coverable nodes scanner. [[view](https://github.com/p4lang/p4c/pull/4203)]
- [P4Testgen] Add a DPDK-PTF P4Testgen back end and the corresponding test runner [[view](https://github.com/p4lang/p4c/pull/4173)]
- ParserUnroll: fix seg. fault for uninitialized variables inside header stack indexes [[view](https://github.com/p4lang/p4c/pull/4200)]
- Match switch labels directly instead of replacing them. Use .action_run return value instead of indirection. [[view](https://github.com/p4lang/p4c/pull/4161)]
- [P4Testgen] Change Protobuf backend test case file extension to `.txtpb` [[view](https://github.com/p4lang/p4c/pull/4201)]
- Ensured that error messages cannot be demoted to info messages [[view](https://github.com/p4lang/p4c/pull/4197)]
- Fixes for the artifact evaluation. [[view](https://github.com/p4lang/p4c/pull/4199)]
- Only look ahead for the else branch when it is not null. Check for null in node coverage. [[view](https://github.com/p4lang/p4c/pull/4191)]
- [P4Testgen] Support string types in the Z3 solver. [[view](https://github.com/p4lang/p4c/pull/4195)]
- testutils: Fix LogPipe to always record output.
- backends/ebpf: Output an error if a header is not byte-aligned [[view](https://github.com/p4lang/p4c/pull/4176)].
- backends/ebpf: Support errors testing.
- Stop endianess conversion in parser [[view](https://github.com/p4lang/p4c/pull/4193)]
- Update BUILD.bazel [[view](https://github.com/p4lang/p4c/pull/4192)]
- Update the Bazel build files [[view](https://github.com/p4lang/p4c/pull/4190)]
- Added support of info messages [[view](https://github.com/p4lang/p4c/pull/4184)]
- Move the solver file from the lib folder to the ir folder. [[view](https://github.com/p4lang/p4c/pull/4188)]
- Do not treat an array index as a purely symbolic variable and resolve the index correctly. [[view](https://github.com/p4lang/p4c/pull/4186)]
- Delete top-level dash-pipeline file. [[view](https://github.com/p4lang/p4c/pull/4187)]
- Run Gauntlet validation, sanitizers, and Ubuntu 18 build/test as nightly actions. [[view](https://github.com/p4lang/p4c/pull/4105)]
- Show annotations in actions' dbprint [[view](https://github.com/p4lang/p4c/pull/4183)]
- Update the dash-pipeline programs. [[view](https://github.com/p4lang/p4c/pull/4060)]
- Simplify the DPDK PTF build and test scripts.  [[view](https://github.com/p4lang/p4c/pull/4168)]
- Encode more P4Runtime constraints for the behavioral model. [[view](https://github.com/p4lang/p4c/pull/4103)]
- [P4Testgen] Add more trace information to P4Testgen [[view](https://github.com/p4lang/p4c/pull/4157)]
- Add an option to only generate tests which improve coverage. [[view](https://github.com/p4lang/p4c/pull/4163)]

## Release v1.2.4.4 [[view](https://github.com/p4lang/p4c/pull/4180)]
- Get host architecture dynamically while downloading `protoc` binary [[view](https://github.com/p4lang/p4c/pull/4179)]
- Change PNA port width to 32 instead of 9. [[view](https://github.com/p4lang/p4c/pull/4177)]
- Build the libbpf library during the cmake configuration process [[view](https://github.com/p4lang/p4c/pull/4166)]
- backends/ebpf: Add support for XDP model. [[view](https://github.com/p4lang/p4c/pull/4160)]
- Dpdk backend: Remove illegal instruction generation, reject operations on >64bit operands [[view](https://github.com/p4lang/p4c/pull/4171)]
- [P4Testgen] Remove untested/unreliable P4Testgen features. [[view](https://github.com/p4lang/p4c/pull/4165)]
- Do not panic when user specifies impossible types for declarations [[view](https://github.com/p4lang/p4c/pull/4169)]
- backends/ebpf: Fix #4098 by renaming conflicting write_partial macro. [[view](https://github.com/p4lang/p4c/pull/4134)]
- Do not allow 'void' for lookahead type [[view](https://github.com/p4lang/p4c/pull/4148)]
- Increase the timeout of the switch.p4 p4_14 program. [[view](https://github.com/p4lang/p4c/pull/4164)]
- [P4Testgen] Minor improvements and fixes for P4Testgen.  [[view](https://github.com/p4lang/p4c/pull/4162)]
- Hashvec-based maps to save memory [[view](https://github.com/p4lang/p4c/pull/4150)]
- Move solver class to lib folder. [[view](https://github.com/p4lang/p4c/pull/4091)]
- Fix whole header move instructions for DPDK [[view](https://github.com/p4lang/p4c/pull/4153)]
- Add error message for non-package main declaration [[view](https://github.com/p4lang/p4c/pull/4141)]
- Do not use equ for mandating a minimum size. [[view](https://github.com/p4lang/p4c/pull/4154)]
- eBPF backend: generate P4Runtime files if required [[view](https://github.com/p4lang/p4c/pull/4113)]
- Allow lookahead of structs with size of 0 [[view](https://github.com/p4lang/p4c/pull/4149)]
- Fix incorrect learn action in case of learner table invoked from nested controls
- Sane handling of P4C_DEFAULT_X envvars in p4c [[view](https://github.com/p4lang/p4c/pull/4130)]
- Separate Parser and Post-parser code C code [[view](https://github.com/p4lang/p4c/pull/4138)]
- Add error message for slice with negative indexes [[view](https://github.com/p4lang/p4c/pull/4137)]
- Fix invalid string format specifier [[view](https://github.com/p4lang/p4c/pull/4145)]
- Perform type inference after constant-folding a code fragment [[view](https://github.com/p4lang/p4c/pull/4135)]
- Fix incorrect installation link in P4Testgen README [[view](https://github.com/p4lang/p4c/pull/4131)]
- Resolve P4tc ebpf c issues [[view](https://github.com/p4lang/p4c/pull/4115)]
- ci/release: enable push [[view](https://github.com/p4lang/p4c/pull/4128)]

## Release v1.2.4.3 [[view](https://github.com/p4lang/p4c/pull/4124)]
- ci/release: create tagged container images [[view](https://github.com/p4lang/p4c/pull/4125)]
- Remove P4Testgen codeowners file. [[view](https://github.com/p4lang/p4c/pull/4119)]
- Migrate Automatic Exec Groups by adding a toolchain parameter to the affected actions [[view](https://github.com/p4lang/p4c/pull/4116)]
- Fix linking with Protobuf 23 [[view](https://github.com/p4lang/p4c/pull/4104)]
- Fix MAC Address endianness issue [[view](https://github.com/p4lang/p4c/pull/4089)]
- Add a comment on a not strictly necessary grammar rule [[view](https://github.com/p4lang/p4c/pull/3016)]
- Use namespace for dpdk PTF tests [[view](https://github.com/p4lang/p4c/pull/4111)]
- Added hooks in p4RuntimeArch and p4RuntimeSerializer [[view](https://github.com/p4lang/p4c/pull/4112)]
- [P4Testgen] Add documentation to P4Testgen [[view](https://github.com/p4lang/p4c/pull/4097)]
- Continuing the implementation of initial entries support in p4c [[view](https://github.com/p4lang/p4c/pull/4080)]
- Minor tweaks [[view](https://github.com/p4lang/p4c/pull/4106)]
- [P4Testgen] Allow building p4testgen without BMV2 target [[view](https://github.com/p4lang/p4c/pull/4109)]
- Use libbacktrace for stack dumps if available [[view](https://github.com/p4lang/p4c/pull/4088)]
- Pull the libbpf library with FetchContent instead of submodules and a script. [[view](https://github.com/p4lang/p4c/pull/3970)]
- Add a PTF test CI pipeline for p4c-dpdk on the DPDK SoftNIC [[view](https://github.com/p4lang/p4c/pull/4072)]
- Migrate STF scripts to the STF parser library. Modernize stale run-bmv2-test.py code. [[view](https://github.com/p4lang/p4c/pull/4040)]
- Fix header stack assignment in p4c-dpdk [[view](https://github.com/p4lang/p4c/pull/4100)]
- [P4Testgen] Add target-specifc P4Testgen gtests [[view](https://github.com/p4lang/p4c/pull/4093)]
- Update GoogleTest and install it via FetchContent [[view](https://github.com/p4lang/p4c/pull/4092)]
- Automated Release v1.2.4.2 [[view](https://github.com/p4lang/p4c/pull/4090)]
- Make sure we ignore warnings in the Protobuf dependency source code. [[view](https://github.com/p4lang/p4c/pull/4086)]
- Update P4Runtime and use FetchContent instead of a submodule. [[view](https://github.com/p4lang/p4c/pull/4082)]
- Fix missing header for intmax_t typedef [[view](https://github.com/p4lang/p4c/pull/4084)]
- Install Protobuf using FetchContent. [[view](https://github.com/p4lang/p4c/pull/4056)]
- Add test program that uses 'priority' as identifier of a few kinds [[view](https://github.com/p4lang/p4c/pull/4079)]
- Allow building testgen without P4TEST enabled [[view](https://github.com/p4lang/p4c/pull/4078)]
- Add an option to the DPDK P4C back end to generate TDI builder configurations. [[view](https://github.com/p4lang/p4c/pull/4068)]
- Try to fix Opensuse certificate errors. [[view](https://github.com/p4lang/p4c/pull/4075)]
- Update the plotting script. [[view](https://github.com/p4lang/p4c/pull/4061)]
- Fix an infinite loop in the coverable nodes scanner. [[view](https://github.com/p4lang/p4c/pull/4073)]
- Fix #4067: Add static casts required by newer versions of G++ [[view](https://github.com/p4lang/p4c/pull/4074)]
- Fix MacOS installation typo. [[view](https://github.com/p4lang/p4c/pull/4071)]

## Release v1.2.4.1 [[view](https://github.com/p4lang/p4c/pull/4052)]
- [P4Testgen] Make the ranges of supported ports a command line parameter for P4Testgen [[view](https://github.com/p4lang/p4c/pull/4069)]
- Allow building the compiler without P4TEST enabled [[view](https://github.com/p4lang/p4c/pull/4065)]
- Avoid using reserved identifiers as include guards [[view](https://github.com/p4lang/p4c/pull/4063)]
- Don't omit the apply block label in P4Control::dbprint [[view](https://github.com/p4lang/p4c/pull/4062)]
- Fix crash in eliminateTuples [[view](https://github.com/p4lang/p4c/pull/4058)]
- Fix parser so that `priority` can be used in expressions [[view](https://github.com/p4lang/p4c/pull/4053)]
- [P4Testgen] More fixes to the refers_to and p4_asserts parser [[view](https://github.com/p4lang/p4c/pull/4051)]
- Add Bazel build rules for P4TestGen [[view](https://github.com/p4lang/p4c/pull/4050)]
- Include annotations for new_type messages. [[view](https://github.com/p4lang/p4c/pull/4047)]
- [P4Testgen] BMv2 test generation improvements [[view](https://github.com/p4lang/p4c/pull/4046)]
- Change Dockerfile to build p4tools by default. [[view](https://github.com/p4lang/p4c/pull/4049)]
- Wrap the TC defines in a namespace, use constexpr. [[view](https://github.com/p4lang/p4c/pull/4048)]
- Fixed tc table type [[view](https://github.com/p4lang/p4c/pull/4045)]
- Cleaner dbprint of if-else if chains [[view](https://github.com/p4lang/p4c/pull/4044)]
- Fix race condition for parallel PTF nanomsg tests by using network namespaces. [[view](https://github.com/p4lang/p4c/pull/4042)]
- [P4Testgen] Control the Z3 dependency in CMake using fetchcontent, generalize FindZ3.cmake [[view](https://github.com/p4lang/p4c/pull/4034)]
- Remove old ipaddr python module [[view](https://github.com/p4lang/p4c/pull/4037)]
- Use a version of simple switch that uses nanomsg instead of virtual interfaces for PTF tests. [[view](https://github.com/p4lang/p4c/pull/3951)]
- Fix incorrect boolean cast in the Z3 solver implementation. [[view](https://github.com/p4lang/p4c/pull/4039)]
- [P4Testgen] Miscellaneous fixes for P4testgen [[view](https://github.com/p4lang/p4c/pull/4036)]
- Remove execution state from the symbolic executor, make it a parameter. Simplify SelectedBranches. [[view](https://github.com/p4lang/p4c/pull/4031)]
- Reduce overtainting by short-circuiting some expressions and extern invocations. [[view](https://github.com/p4lang/p4c/pull/4030)]
- Hotfix for failing MacOS tests. Pin boost.  [[view](https://github.com/p4lang/p4c/pull/4027)]
- Dpdk Backend: Fixed target_name and action parameter bitwidth [[view](https://github.com/p4lang/p4c/pull/4025)]
- Remove the array indices conversions and handle indices in the interpreter. [[view](https://github.com/p4lang/p4c/pull/4024)]
- [P4Testgen] Simplify the model [[view](https://github.com/p4lang/p4c/pull/4021)]
- p4c-pna-p4tc : new TC backend for p4c [[view](https://github.com/p4lang/p4c/pull/4018)]
- [P4Testgen] Remove complete from the model, make it part of the evaluation step instead.  [[view](https://github.com/p4lang/p4c/pull/4015)]
- Implement support for non-const table entries [[view](https://github.com/p4lang/p4c/pull/3748)]
- Also suppress no-gnu-zero-variadic-macro-arguments warnings for MacOs. [[view](https://github.com/p4lang/p4c/pull/4023)]

## Release v1.2.4
- [P4Testgen] Open up P4Testgen interface.  [[view](https://github.com/p4lang/p4c/pull/4014)]
- [P4Testgen] Create abstract execution state with utility functions - move copy-in/out to control/parser blocks. [[view](https://github.com/p4lang/p4c/pull/4011)]
- cmake: Fix include search for Z3 in testgen library [[view](https://github.com/p4lang/p4c/pull/4012)]
- [P4Testgen] Move some table code into helper functions, move gen_eq to "common" folder [[view](https://github.com/p4lang/p4c/pull/4010)]
- Properly track indices when removing parser value sets from the select cases [[view](https://github.com/p4lang/p4c/pull/4008)]
- Open up ResolutionContext interface a bit [[view](https://github.com/p4lang/p4c/pull/4009)]
- Add a simple PSA test program and STF test to verify parser_error ope??? [[view](https://github.com/p4lang/p4c/pull/2571)]
- lib/crash.cpp: `addr2line`: `fcntl` calls in `pipe` initializations fixed. [[view](https://github.com/p4lang/p4c/pull/3298)]
- [P4Testgen] More P4Tools refactoring work. [[view](https://github.com/p4lang/p4c/pull/4005)]
- [P4Testgen] Fix small issues with metadata test backend. Add more functions to IR utilities. Merge default value function. [[view](https://github.com/p4lang/p4c/pull/4004)]
- [P4Testgen] Also include table entries in the coverage set. [[view](https://github.com/p4lang/p4c/pull/3999)]
- Allowing local_copyprop to propagate MethodCallExpression into table ??? [[view](https://github.com/p4lang/p4c/pull/4003)]
- [P4Testgen] Fix include paths [[view](https://github.com/p4lang/p4c/pull/3997)]
- [P4Testgen] More StateVariable refactoring. Allow PathExpression to be state variables.  [[view](https://github.com/p4lang/p4c/pull/3990)]
- Make P4_16 the default parser option. [[view](https://github.com/p4lang/p4c/pull/4002)]
- misc helper functions [[view](https://github.com/p4lang/p4c/pull/3995)]
- NVFMSA-2399 Fix initialization-order-fiasco of P4CoreLibrary instance [[view](https://github.com/p4lang/p4c/pull/3994)]
- [P4Testgen] Implement meter support for the BMv2 V1model PTF test back end [[view](https://github.com/p4lang/p4c/pull/3974)]

## Release v1.2.3.9 [[view](https://github.com/p4lang/p4c/pull/3998)]
- [P4Testgen] Clean up the implementation of the BMv2 clone externs.  [[view](https://github.com/p4lang/p4c/pull/3976)]
- Split state variables and symbolic variables. Stricter type checking. [[view](https://github.com/p4lang/p4c/pull/3987)]
- Run linters across test folder. [[view](https://github.com/p4lang/p4c/pull/3982)]
- Add some more type information to the strength reduction pass. [[view](https://github.com/p4lang/p4c/pull/3993)]
- FreeBSD patches [[view](https://github.com/p4lang/p4c/pull/3991)]
- Also include the backend IR files in the ir-generated library file. [[view](https://github.com/p4lang/p4c/pull/3985)]
- Set up P4C for IWYU [[view](https://github.com/p4lang/p4c/pull/3981)]
- Make StateVariable an IR class that accepts IR::Member. Use ICastable for some nodes. [[view](https://github.com/p4lang/p4c/pull/3741)]
- Cleanup some P4Testgen code.  [[view](https://github.com/p4lang/p4c/pull/3978)]
- Bump Bazel dependency versions to enable using latest Bazel [[view](https://github.com/p4lang/p4c/pull/3979)]
- Add style and formating files for Python. Format Python code. [[view](https://github.com/p4lang/p4c/pull/3870)]
- Implement support for default initializers (...)] [[view](https://github.com/p4lang/p4c/pull/3968)]
- Always insert instantiation statement at the end of the local statement list [[view](https://github.com/p4lang/p4c/pull/3962)]
- ir generator fixes + cleanup [[view](https://github.com/p4lang/p4c/pull/3975)]
- initialize json pointer (coverity fix)] [[view](https://github.com/p4lang/p4c/pull/3972)]
- ebpf/PSA: Fix weekly PTF tests [[view](https://github.com/p4lang/p4c/pull/3969)]
- Resolve types in the prepend internal extern. [[view](https://github.com/p4lang/p4c/pull/3964)]
- Change array index[] in table keys in context.json to $ to match with the key name in bfrt.json [[view](https://github.com/p4lang/p4c/pull/3963)]
- Clean up the trace event class of P4Tools.  [[view](https://github.com/p4lang/p4c/pull/3950)]
- Remove gsl-lite module. Introduce std::referencewrapper. [[view](https://github.com/p4lang/p4c/pull/3961)]
- Remove the inja submodule and replace it with a CMake fetchcontent instrumentation. [[view](https://github.com/p4lang/p4c/pull/3959)]
- Remove boost variant from Tools. [[view](https://github.com/p4lang/p4c/pull/3960)]
- Fix the following issues [[view](https://github.com/p4lang/p4c/pull/3958)]

## Release v1.2.3.8 [[view](https://github.com/p4lang/p4c/pull/3957)]
- Add documentation on adding test data [[view](https://github.com/p4lang/p4c/pull/3954)]
- Make cpplint use python3 explicitely. [[view](https://github.com/p4lang/p4c/pull/3956)]
- Eliminate typedefs when resolving constructor type declaration [[view](https://github.com/p4lang/p4c/pull/3953)]
- Run IWYU across tools again to clean up some includes.  [[view](https://github.com/p4lang/p4c/pull/3940)]
- Better logging for lists/sets of things that take multiple lines
- Implement a PNA-DPDK extension with a metadata test back end for P4Testgen [[view](https://github.com/p4lang/p4c/pull/3937)]
- Introduce stack initializers [[view](https://github.com/p4lang/p4c/pull/3803)]
- Update cpplint.py [[view](https://github.com/p4lang/p4c/pull/3941)]
- Clean up the commit hook installation script. [[view](https://github.com/p4lang/p4c/pull/3947)]
- Replace boost::optional with std::optional. [[view](https://github.com/p4lang/p4c/pull/3942)]
- Remove boost::filesystem from P4Testgen. [[view](https://github.com/p4lang/p4c/pull/3944)]
- Support for invalid union expressions [[view](https://github.com/p4lang/p4c/pull/3843)]
- Dash reference files. [[view](https://github.com/p4lang/p4c/pull/3943)]
- Add a metadata collection mode. [[view](https://github.com/p4lang/p4c/pull/3933)]
- Remove predication pass from p4test [[view](https://github.com/p4lang/p4c/pull/3916)]
- Fix context generation when both --context and --bf-rt-schema options are supplied [[view](https://github.com/p4lang/p4c/pull/3936)]
- Direct cast is possible between equivalent types [[view](https://github.com/p4lang/p4c/pull/3935)]
- p4tool: Fix build with Z3 installed in non-standard path [[view](https://github.com/p4lang/p4c/pull/3932)]
- testgen: Add missing include [[view](https://github.com/p4lang/p4c/pull/3931)]
- Rename some exploration strategies - Simplify DFS and random selection [[view](https://github.com/p4lang/p4c/pull/3918)]
- Use Ubuntu 22.04 for Testgen PTF tests.  [[view](https://github.com/p4lang/p4c/pull/3923)]
- Try out a simpler advance expression calculation. [[view](https://github.com/p4lang/p4c/pull/3926)]
- Third party libraries are now included as SYSTEM [[view](https://github.com/p4lang/p4c/pull/3924)]
- Refactor P4Testgen strategies - Part 1. Add a new strategy - greedy search. [[view](https://github.com/p4lang/p4c/pull/3878)]
- Add an explicit optional match key type. [[view](https://github.com/p4lang/p4c/pull/3920)]
- Add the DASH BMv2 model to the tested programs. [[view](https://github.com/p4lang/p4c/pull/3885)]
- Remove @vhavel from the list of P4Testgen code owners. [[view](https://github.com/p4lang/p4c/pull/3919)]
- Dpdk Backend: Add support for assignments to and from large (>64-bit)] operand [[view](https://github.com/p4lang/p4c/pull/3910)]
- ebpf/PSA: Add support for wide fields in parser value_set [[view](https://github.com/p4lang/p4c/pull/3912)]
- Fix mask calculation for concat operator [[view](https://github.com/p4lang/p4c/pull/3911)]
- Add an explanation for unity builds. Add a deprecation warning. [[view](https://github.com/p4lang/p4c/pull/3914)]
- Resolve parser states at once instead of resolving them iteratively. [[view](https://github.com/p4lang/p4c/pull/3875)]
- Do not allow arguments with type table, control, etc [[view](https://github.com/p4lang/p4c/pull/3901)]
- CMake build tweaks [[view](https://github.com/p4lang/p4c/pull/3907)]
- Rework testutils.py [[view](https://github.com/p4lang/p4c/pull/3899)]

## Release v1.2.3.7 [[view](https://github.com/p4lang/p4c/pull/3909)]
- Test support for wide fields in ActionProfile and ActionSelector [[view](https://github.com/p4lang/p4c/pull/3908)]
- Fix up the refers_to parser. [[view](https://github.com/p4lang/p4c/pull/3906)]
- ebpf/PSA: Support for wide fields in Digest [[view](https://github.com/p4lang/p4c/pull/3877)]
- Fix varbit handling [[view](https://github.com/p4lang/p4c/pull/3761)]
- SplitFlowVisitor improvements
- Deprecate unified build in favor of unity build. [[view](https://github.com/p4lang/p4c/pull/3491)]
- Collect and flatten local struct decls [[view](https://github.com/p4lang/p4c/pull/3903)]
- Fix warnings in compilation using clang 15 [[view](https://github.com/p4lang/p4c/pull/3904)]
- Add support for toeplitz hash [[view](https://github.com/p4lang/p4c/pull/3897)]
- Add support for flattening header union stack [[view](https://github.com/p4lang/p4c/pull/3890)]
- Container arguments with type infint are treated as type variables [[view](https://github.com/p4lang/p4c/pull/3889)]
- Build with -pedantic, fix problems [[view](https://github.com/p4lang/p4c/pull/3892)]
- CI: Find protobuf and pass it to cmake on macOS [[view](https://github.com/p4lang/p4c/pull/3896)]
- Install & use GCC 9 in Ubuntu 18 build [[view](https://github.com/p4lang/p4c/pull/3893)]
- add a feature to control logging via an annotation on annotated IR ob??? [[view](https://github.com/p4lang/p4c/pull/3882)]
- Add assert and assume mode for testgen. [[view](https://github.com/p4lang/p4c/pull/3775)]
- SplitFlowVisitor to better capture split/join in ControlFlowVisitor
- Replace invalid clone id 0 with 1 for selected BMv2 programs [[view](https://github.com/p4lang/p4c/pull/3860)]
- Fix up one-shot-action-selectors with action arguments. Appropriately rename some P4 table programs. [[view](https://github.com/p4lang/p4c/pull/3864)]
- Fix bug in parserUnroll [[view](https://github.com/p4lang/p4c/pull/3874)]

## Release v1.2.3.6 [[view](https://github.com/p4lang/p4c/pull/3871)]
- log: Add a format_container helper & use it [[view](https://github.com/p4lang/p4c/pull/3872)]
- Unbounded random strategy to pick a branch for test generation [[view](https://github.com/p4lang/p4c/pull/3873)]
- Dpdk Backend: Add IPSec support  [[view](https://github.com/p4lang/p4c/pull/3858)]
- Update test to be backend agnostic by removing allocation specific code for match groups [[view](https://github.com/p4lang/p4c/pull/3869)]
- Refactor Python testing scripts and utilities - Part 1  [[view](https://github.com/p4lang/p4c/pull/3855)]
- Move p4runtime arch handler class declaration to header file [[view](https://github.com/p4lang/p4c/pull/3867)]
- Fix flawed code in up4.p4 and the pins switch models. [[view](https://github.com/p4lang/p4c/pull/3857)]
- Fix hardcoded tests line. [[view](https://github.com/p4lang/p4c/pull/3856)]
- ebpf/PSA: Support for wide fields in Register and Meter [[view](https://github.com/p4lang/p4c/pull/3854)]
 - Support "clone" function for BMV2 PTF [[view](https://github.com/p4lang/p4c/pull/3848)]
- [P4testgen] Coverage script [[view](https://github.com/p4lang/p4c/pull/3853)]
- Simple proof of concept PTF test for BMV2 test framework. [[view](https://github.com/p4lang/p4c/pull/3849)]
- Emit action sel id as in bfrt [[view](https://github.com/p4lang/p4c/pull/3852)]
- Require action_run to be within a switch statement [[view](https://github.com/p4lang/p4c/pull/3850)]
- Table application results are not comparable [[view](https://github.com/p4lang/p4c/pull/3851)]
 - Correction of PTF tests for BMV2 [[view](https://github.com/p4lang/p4c/pull/3737)]
- Fix ckadd argument [[view](https://github.com/p4lang/p4c/pull/3845)]
- Fix clang formatting
- Streamline seed initialization and try to minimize nondeterminism in P4Testgen. [[view](https://github.com/p4lang/p4c/pull/3839)]
- Add stop metrics for P4Testgen. Sanitize some P4Testgen options. [[view](https://github.com/p4lang/p4c/pull/3829)]
- Avoiding choosing the same port as egress port for clone. [[view](https://github.com/p4lang/p4c/pull/3844)]
- Testgen: allow block param initialization with taintexpr [[view](https://github.com/p4lang/p4c/pull/3840)]
- Fix clang_format mismatch. [[view](https://github.com/p4lang/p4c/pull/3831)]
- Cannot compare externs [[view](https://github.com/p4lang/p4c/pull/3834)]
- Make {#} a single token [[view](https://github.com/p4lang/p4c/pull/3835)]
- Experimental: support for trailing commas [[view](https://github.com/p4lang/p4c/pull/3804)]
- Unify ctxt json's id with other runtime files [[view](https://github.com/p4lang/p4c/pull/3833)]
- Drop flawed input-packet-only option. [[view](https://github.com/p4lang/p4c/pull/3830)]
- [dpdk] Emit a pseudo header, for dpdk instructions which require [[view](https://github.com/p4lang/p4c/pull/3807)]
- Simplify DoConstantFolding::postorder [[view](https://github.com/p4lang/p4c/pull/3805)]
- Not all Type_InfInt are the same - keep track of ones that need to be unified [[view](https://github.com/p4lang/p4c/pull/3828)]
- Forbid @optional parameters in some places [[view](https://github.com/p4lang/p4c/pull/3827)]
- Make ports an int32 type. [[view](https://github.com/p4lang/p4c/pull/3825)]
- Equal types are always equivalent [[view](https://github.com/p4lang/p4c/pull/3707)]
- Fix IndexedVector methods and constructor [[view](https://github.com/p4lang/p4c/pull/3821)]
- Refactor ci-build.sh script. [[view](https://github.com/p4lang/p4c/pull/3792)]
- Move timer from testgen to lib folder. [[view](https://github.com/p4lang/p4c/pull/3785)]
- Utility edits for P4Tools. [[view](https://github.com/p4lang/p4c/pull/3824)]
- Update inja and fix inja formatting. [[view](https://github.com/p4lang/p4c/pull/3782)]
- Dpdk Backend : Do not treat BAnd expresion in table key as simple expression [[view](https://github.com/p4lang/p4c/pull/3823)]
- initialize direction metadata at begining of pipeline [[view](https://github.com/p4lang/p4c/pull/3817)]
- Fix for STF tests with BMv2 when Scapy 2.5.0 is installed [[view](https://github.com/p4lang/p4c/pull/3816)]

## Release v1.2.3.5 [[view](https://github.com/p4lang/p4c/pull/3815)]
- Use big_int for match_t so can match wider that 64 bits
- ebpf/PSA: Checksum support for fields wider than 64 bits [[view](https://github.com/p4lang/p4c/pull/3801)]
- Constant-fold isValid calls when possible [[view](https://github.com/p4lang/p4c/pull/3794)]
- Always emit the action selector constants in hex [[view](https://github.com/p4lang/p4c/pull/3800)]
- Forbid generic externs for in parameter values [[view](https://github.com/p4lang/p4c/pull/3793)]
- ebpf/PSA: improve support for wide fields [[view](https://github.com/p4lang/p4c/pull/3790)]
- Use https with opensuse repositories [[view](https://github.com/p4lang/p4c/pull/3797)]
- Remove permissions. [[view](https://github.com/p4lang/p4c/pull/3791)]
- Fix bazel version to 5.4.0 [[view](https://github.com/p4lang/p4c/pull/3796)]
- ci: update actions [[view](https://github.com/p4lang/p4c/pull/3789)]
- Build (only)] container images for new PRs [[view](https://github.com/p4lang/p4c/pull/3788)]
- Generate warning when 0 tests are produced [[view](https://github.com/p4lang/p4c/pull/3784)]
- Fix container build [[view](https://github.com/p4lang/p4c/pull/3786)]
- Run build and tests directly on Ubuntu 20.04, 22.04 without docker [[view](https://github.com/p4lang/p4c/pull/3770)]
- eBPF/PSA: add support for parser input metadata [[view](https://github.com/p4lang/p4c/pull/3778)]
- Run MacOS tests in correct directory. [[view](https://github.com/p4lang/p4c/pull/3718)]
- Option to produce tests exclusively with output packets [[view](https://github.com/p4lang/p4c/pull/3783)]
- Fix non-contiguous table keys when keys come from different header and metadata struct but existing metadata keys are already contiguous [[view](https://github.com/p4lang/p4c/pull/3780)]
- Remove backends from CMake include paths.  [[view](https://github.com/p4lang/p4c/pull/3769)]
- Check whether a test back end has been provided. [[view](https://github.com/p4lang/p4c/pull/3777)]
- Dpdk Backend: Fix action selector and action profile to apply the group/member tables only on base table hit [[view](https://github.com/p4lang/p4c/pull/3758)]
- Fixed missing invocation of Address Sanitizer [[view](https://github.com/p4lang/p4c/pull/3735)]
- Ignore formatting PR from blame. [[view](https://github.com/p4lang/p4c/pull/3766)]
- Add missing semicolon at end of Bison rule in p4parser.ypp [[view](https://github.com/p4lang/p4c/pull/3764)]
- Apply clang-format to ir and lib files. [[view](https://github.com/p4lang/p4c/pull/3759)]
- Fix non-consecutive table keys for add_on_miss table [[view](https://github.com/p4lang/p4c/pull/3763)]
- Refactor add_cpplint_files and add_clang_format files to support downstream projects, large file numbers. [[view](https://github.com/p4lang/p4c/pull/3762)]
- Switch cases can fall through. [[view](https://github.com/p4lang/p4c/pull/3756)]
- Check that value sets have a constant constructor argument [[view](https://github.com/p4lang/p4c/pull/3754)]
- PSA/ebpf backend: add support for generating programs for XDP hook [[view](https://github.com/p4lang/p4c/pull/3752)]
- Raise minimum required CMake version to Ubuntu 18.04 version. [[view](https://github.com/p4lang/p4c/pull/3745)]
- Clean up the includes in the IR folder [[view](https://github.com/p4lang/p4c/pull/3701)]
- Disallow Type_Control as a tuple argument [[view](https://github.com/p4lang/p4c/pull/3753)]

## Release v1.2.3.4 [[view](https://github.com/p4lang/p4c/pull/3747)]
- Propagate def files upwards in the correct order from extensions and modules. [[view](https://github.com/p4lang/p4c/pull/3744)]
- List contributors to P4Tools backend [[view](https://github.com/p4lang/p4c/pull/3742)]
- eBPF/PSA: Implement caching for ActionSelector, LPM and ternary tables [[view](https://github.com/p4lang/p4c/pull/3738)]
- Unroll finite loops in programs which have infinite loops. [[view](https://github.com/p4lang/p4c/pull/3689)]
- Try to merge copyHeaders into copyStructures. [[view](https://github.com/p4lang/p4c/pull/3739)]
- Add support for register extern [[view](https://github.com/p4lang/p4c/pull/3723)]
- Move some P4Tool compiler passes to the midend folder. [[view](https://github.com/p4lang/p4c/pull/3726)]
- Small clang-format and P4Tools README fixes. [[view](https://github.com/p4lang/p4c/pull/3736)]
- Fixed issues found by static analysis [[view](https://github.com/p4lang/p4c/pull/3732)]
- Expand google.protobuf.Any when emit p4info in text format [[view](https://github.com/p4lang/p4c/pull/3731)]
- Remove unused files in p4_16_samples_outputs directory [[view](https://github.com/p4lang/p4c/pull/3733)]
- Emit table config [[view](https://github.com/p4lang/p4c/pull/3710)]
- Enable auto var initialization with pattern in Clang CI build [[view](https://github.com/p4lang/p4c/pull/3729)]
- Defensive programming suggested by static analysis - check iterator bounds [[view](https://github.com/p4lang/p4c/pull/3725)]
- Method calls in the actions list must invoke actions [[view](https://github.com/p4lang/p4c/pull/3728)]
- Arithmetic and bitwise operations are typechecked the same way [[view](https://github.com/p4lang/p4c/pull/3716)]
- Off by one warning error in constant-folding for large shifts [[view](https://github.com/p4lang/p4c/pull/3715)]
- Fix bug caused by uninitialized variable in typechecker [[view](https://github.com/p4lang/p4c/pull/3720)]
- Fix advance condition calculation. [[view](https://github.com/p4lang/p4c/pull/3721)]
- P4Testgen: Fix off-by-one bug in taint checking for slices [[view](https://github.com/p4lang/p4c/pull/3722)]
- PSA/eBPF: Enable passing packets up to the kernel stack [[view](https://github.com/p4lang/p4c/pull/3691)]
- Restructure P4Tools to easily add new modules. [[view](https://github.com/p4lang/p4c/pull/3646)]
- P4Tools: Unify packet sizing options. [[view](https://github.com/p4lang/p4c/pull/3695)]
- P4Tools: Implement default action override for BMv2 STF. [[view](https://github.com/p4lang/p4c/pull/3685)]
- Try to avoid gcc-specific include headers for IWYU. [[view](https://github.com/p4lang/p4c/pull/3717)]
- Do not allow type arguments that are generic if they are not specialized [[view](https://github.com/p4lang/p4c/pull/3714)]
- Run IWYU on P4Tools. [[view](https://github.com/p4lang/p4c/pull/3678)]
- Dags needs to be revisited when renaming metadata [[view](https://github.com/p4lang/p4c/pull/3713)]
- Add PINS examples to test suite. [[view](https://github.com/p4lang/p4c/pull/3694)]
- Linter usability improvements. [[view](https://github.com/p4lang/p4c/pull/3709)]
- Add ignore revs file for clang-format PR. [[view](https://github.com/p4lang/p4c/pull/3712)]
- Alternate strength reduction and constant folding to eliminate constants [[view](https://github.com/p4lang/p4c/pull/3706)]
- Support for list types and literals [[view](https://github.com/p4lang/p4c/pull/3520)]
- Enforce clang-format on the entire code base. [[view](https://github.com/p4lang/p4c/pull/3679)]
- Cleanup warnings produced by Clang [[view](https://github.com/p4lang/p4c/pull/3693)]
- Fix name shortening [[view](https://github.com/p4lang/p4c/pull/3692)]
- Add a dedicated target for cpplint, if no files are set. [[view](https://github.com/p4lang/p4c/pull/3697)]
- Update p4_library.bzl [[view](https://github.com/p4lang/p4c/pull/3698)]
- Correctly infer argument types for constructor calls [[view](https://github.com/p4lang/p4c/pull/3686)]
- Enabled permissive by default. [[view](https://github.com/p4lang/p4c/pull/3690)]
- PSA/eBPF: use slice-by-8 algorithm for CRC-32 [[view](https://github.com/p4lang/p4c/pull/3688)]
- create dpdk specific pna.p4 and extend it  [[view](https://github.com/p4lang/p4c/pull/3658)]
- Fix tmpMask alignment [[view](https://github.com/p4lang/p4c/pull/3687)]
- Insert casts in action call arguments [[view](https://github.com/p4lang/p4c/pull/3675)]
- Fix dpdk test infrastructure to enable comparison of error output [[view](https://github.com/p4lang/p4c/pull/3680)]
- Experimental: literal for invalid header [[view](https://github.com/p4lang/p4c/pull/3667)]
- Add a p4-constraints and @refers_to parser to P4Tools.  [[view](https://github.com/p4lang/p4c/pull/3568)]
- Update PSA include files to match recent changes in version from PSA spec [[view](https://github.com/p4lang/p4c/pull/3618)]
- Fix cpplint CMake check. [[view](https://github.com/p4lang/p4c/pull/3684)]
- Rename psabpf repository into NIKSS [[view](https://github.com/p4lang/p4c/pull/3677)]
- Initialize drop metadata to 1 as per PSA specification [[view](https://github.com/p4lang/p4c/pull/3681)]
- Migrate P4Testgen tooling to core P4C (IWYU, clang-format, cpplint, clang-tidy, git hooks)] [[view](https://github.com/p4lang/p4c/pull/3663)]
- Build DCG if pattern flag was enabled [[view](https://github.com/p4lang/p4c/pull/3673)]
- DPDK Backend : Minor fixes [[view](https://github.com/p4lang/p4c/pull/3674)]
- Make RemoveUnusedDeclarations constructor protected. [[view](https://github.com/p4lang/p4c/pull/3670)]
- Use vector to populate checksum struct [[view](https://github.com/p4lang/p4c/pull/3668)]
- Remove useless grammar rule [[view](https://github.com/p4lang/p4c/pull/3664)]
- DPDK Backend: Direct counter and direct meter support [[view](https://github.com/p4lang/p4c/pull/3631)]
- 'size' is always a legal table property [[view](https://github.com/p4lang/p4c/pull/3662)]
- Do not allow action invocations with type arguments [[view](https://github.com/p4lang/p4c/pull/3661)]
- Clone empty blocks before def-use analysis [[view](https://github.com/p4lang/p4c/pull/3653)]
- Tighten checks for NoAction [[view](https://github.com/p4lang/p4c/pull/3651)]
- Check if 'match_kind.exact' is defined when expanding switch statements [[view](https://github.com/p4lang/p4c/pull/3654)]
- PSA/eBPF: retrieve interface numbers from OS in PTF tests  [[view](https://github.com/p4lang/p4c/pull/3659)]
- Fix and improve ordered_map & ordered_set [[view](https://github.com/p4lang/p4c/pull/3577)]
- Handle correctly general switch statements when simplifying [[view](https://github.com/p4lang/p4c/pull/3624)]
- Allow implicit casts in switch labels; improve error message [[view](https://github.com/p4lang/p4c/pull/3634)]

## Release v1.2.3.3 [[view](https://github.com/p4lang/p4c/pull/3648)]
- CI: add sanitizers [[view](https://github.com/p4lang/p4c/pull/3625)]
- Fix compilation issues after reachability merge [[view](https://github.com/p4lang/p4c/pull/3649)]
- Reachability Engine for P4Tools [[view](https://github.com/p4lang/p4c/pull/3609)]
- Fix incorrect tests : totalLen field of ipv4 header, action name in add_entry call [[view](https://github.com/p4lang/p4c/pull/3647)]
- Tighten p4c for add-on-miss [[view](https://github.com/p4lang/p4c/pull/3614)]
- Update hex constants to use uppercase [[view](https://github.com/p4lang/p4c/pull/3645)]
- Migrate some P4Testgen utilities to lib folder. [[view](https://github.com/p4lang/p4c/pull/3630)]
- Add more code owners. [[view](https://github.com/p4lang/p4c/pull/3629)]
- Modify test script running PTF tests for PSA/eBPF [[view](https://github.com/p4lang/p4c/pull/3643)]
- Fix PTF eBPF weekly CI job [[view](https://github.com/p4lang/p4c/pull/3642)]
- Update comments in issue2201-bmv2.p4 to match current behavior [[view](https://github.com/p4lang/p4c/pull/3637)]
- Rename .stf file to match corresponding default_action_ubpf.p4 file [[view](https://github.com/p4lang/p4c/pull/3638)]
- Do not constant fold mux before it has been typechecked [[view](https://github.com/p4lang/p4c/pull/3633)]
- An action profile implementation for v1model protobuf. [[view](https://github.com/p4lang/p4c/pull/3626)]
- Remove GMP support.  [[view](https://github.com/p4lang/p4c/pull/3485)]
- Fix typo in test program for reachability tests.  [[view](https://github.com/p4lang/p4c/pull/3628)]
- Clean up two desynced tests. [[view](https://github.com/p4lang/p4c/pull/3605)]
- switch labels must be path expressions [[view](https://github.com/p4lang/p4c/pull/3615)]
- Dpdk Backend: Minor changes to add error check for mask operands and adding optional match kind to pna.p4 [[view](https://github.com/p4lang/p4c/pull/3588)]
- Allow serenum members initializers refer to other serenum members [[view](https://github.com/p4lang/p4c/pull/3617)]
- Remove unused expected output files -- their test programs were renamed [[view](https://github.com/p4lang/p4c/pull/3621)]
- CI: added build with clang [[view](https://github.com/p4lang/p4c/pull/3607)]
- Remove unncessary v1model include in P4 test programs - fix up programs that use v1model externs. [[view](https://github.com/p4lang/p4c/pull/3608)]
- Fix extra toP4 whitespace after annotations and declaration instances. [[view](https://github.com/p4lang/p4c/pull/3606)]
- Run def-use analysis again after inlining  [[view](https://github.com/p4lang/p4c/pull/3591)]
- Disable libcall optimizations to avoid infinite loops in malloc [[view](https://github.com/p4lang/p4c/pull/3586)]
- Add missing select-struct.p4 test program to p4_16_samples directory [[view](https://github.com/p4lang/p4c/pull/3600)]
- Remove obsolete expected output files issue1409*.p4 [[view](https://github.com/p4lang/p4c/pull/3601)]
- Remove unused expected output files key_ebpf-issue-1020*.p4 [[view](https://github.com/p4lang/p4c/pull/3602)]
- Add missing test program precedence-lt.p4 [[view](https://github.com/p4lang/p4c/pull/3603)]
- Add missing test program fwd-bmv2-psa.p4 [[view](https://github.com/p4lang/p4c/pull/3604)]
- Add missing P4 test program issue2495-bmv2.p4 [[view](https://github.com/p4lang/p4c/pull/3599)]
- Remove unused expected output file [[view](https://github.com/p4lang/p4c/pull/3598)]
- Remove unused expected output file [[view](https://github.com/p4lang/p4c/pull/3597)]
- Remove expected output for non-existent action_param_serenum.p4 test program [[view](https://github.com/p4lang/p4c/pull/3596)]
- Tables cannot be compared [[view](https://github.com/p4lang/p4c/pull/3590)]
- Remove header field size constraints, now [[view](https://github.com/p4lang/p4c/pull/3579)]
- Implement the eBPF extension for P4Tools' P4Testgen [[view](https://github.com/p4lang/p4c/pull/3510)]
- [p4Tools and p4Testgen] New exploration strategy for test generation [[view](https://github.com/p4lang/p4c/pull/3566)]
- Fix taint analysis that was too conservative. [[view](https://github.com/p4lang/p4c/pull/3583)]
- Dpdk backend: Support for large keysize >64 bytes with additional restrictions [[view](https://github.com/p4lang/p4c/pull/3580)]
- made "errorCount", "warningCount", and "maxErrorCount" [all "unsigned int"] be "protected" rather than "private" in "class ErrorReporter" in "error_reporter.h"
- Fix shift overlimit error for valid bit slicing operation [[view](https://github.com/p4lang/p4c/pull/3578)]
- Add a reachability analysis to P4Tools [[view](https://github.com/p4lang/p4c/pull/3569)].
- Introduce cmake option to enable -Werror; use it for CI/docker builds [[view](https://github.com/p4lang/p4c/pull/3555)]
- Add CODEOWNERS file for P4Tools. [[view](https://github.com/p4lang/p4c/pull/3567)]
- More fixes to the P4Tools repository. [[view](https://github.com/p4lang/p4c/pull/3565)]
- Cpplint fixup. [[view](https://github.com/p4lang/p4c/pull/3564)]
- Switch to macOS 11 [[view](https://github.com/p4lang/p4c/pull/3556)]
- Several small compilation fixes for tools. [[view](https://github.com/p4lang/p4c/pull/3558)]
- Update gitmodule to fix cloning with https [[view](https://github.com/p4lang/p4c/pull/3562)]
- add ICastable interface to Backtrack::trigger [[view](https://github.com/p4lang/p4c/pull/3561)]
- Contribute the P4Tools platform and P4Testgen  [[view](https://github.com/p4lang/p4c/pull/3495)]
- Fixing bug for ParserUnroll application to a p4 program with a header union [[view](https://github.com/p4lang/p4c/pull/3121)]
- fixed spelling in CMakelists.txt in 2 different lines [[view](https://github.com/p4lang/p4c/pull/3554)]
- Switch to C++17 standard [[view](https://github.com/p4lang/p4c/pull/3547)]
- Use prefixedNonTypeName to simplify the expression production in parser [[view](https://github.com/p4lang/p4c/pull/3553)]
- Entries can be implicitly cast to the key type [[view](https://github.com/p4lang/p4c/pull/3551)]
- BMv2: fix hashing non-tuple causing segfault [[view](https://github.com/p4lang/p4c/pull/3465)]
- PSA/ebpf backed: update psabpf-ctl to the latest version [[view](https://github.com/p4lang/p4c/pull/3539)]
- Change warrning messages in ParserUnroll [[view](https://github.com/p4lang/p4c/pull/3548)]
- New class ICastable; new type constraint added [[view](https://github.com/p4lang/p4c/pull/3542)]
- Add note to DPDK README that PSA egress is not implemented [[view](https://github.com/p4lang/p4c/pull/3525)]

## Release v1.2.3.2 [[view](https://github.com/p4lang/p4c/pull/3546)]
- Run PTF tests under different kernel versions [[view](https://github.com/p4lang/p4c/pull/3535)]
- add set_member_id in action list for action selector [[view](https://github.com/p4lang/p4c/pull/3540)]
- Dpdk backend: Flatten header union into individual fields [[view](https://github.com/p4lang/p4c/pull/3524)]
- Set C++ standard only if it is not set at parent scope [[view](https://github.com/p4lang/p4c/pull/3541)]
- Fixing bug in ParserUnroll for infinite loops without header stacks  [[view](https://github.com/p4lang/p4c/pull/3538)]
- remove unused lib/alloc.h
- Constant-fold static_assert calls [[view](https://github.com/p4lang/p4c/pull/3533)]
- Dpdk Backend: Update default timeout values for learner table entries [[view](https://github.com/p4lang/p4c/pull/3530)]
- Since dpdk now Support large struct fields(more than 64 bits)] as key [[view](https://github.com/p4lang/p4c/pull/3528)]
- Fix hdr align [[view](https://github.com/p4lang/p4c/pull/3521)]
- Forbid use of .lastIndex outside of parsers [[view](https://github.com/p4lang/p4c/pull/3523)]
- Fixing bug in parserUnroll [[view](https://github.com/p4lang/p4c/pull/3503)]
- Fix: prevent copy elimination for statement which does not support constant values [[view](https://github.com/p4lang/p4c/pull/3512)]
- Fixes related to type and function specialization [[view](https://github.com/p4lang/p4c/pull/3458)]
- Prototype implementation of static_assert [[view](https://github.com/p4lang/p4c/pull/3196)]
- Silence -Warray-bounds for generated code for protobuf [[view](https://github.com/p4lang/p4c/pull/3515)]
- cmake: Fixed multi line comment [[view](https://github.com/p4lang/p4c/pull/3513)]
- Add new option to build p4c with LTO [[view](https://github.com/p4lang/p4c/pull/3470)]
- Load table action from json without 'name' field for BMV2 [[view](https://github.com/p4lang/p4c/pull/3435)]
- Add null check for default action property [[view](https://github.com/p4lang/p4c/pull/3500)]

## Release v1.2.3.1 [[view](https://github.com/p4lang/p4c/pull/3505)]
- Update protobuf installation instructions link [[view](https://github.com/p4lang/p4c/pull/3501)]
- Handle table.apply()].hit correctly in ubpf backend [[view](https://github.com/p4lang/p4c/pull/3498)]
- fixing bug for loop with one parser state only [[view](https://github.com/p4lang/p4c/pull/3492)]
- Update README.md [[view](https://github.com/p4lang/p4c/pull/3497)]
- Dpdk Backend: Fix support for non-zero action arguments for default action in a table using action selector [[view](https://github.com/p4lang/p4c/pull/3496)]
- Fix action param processing [[view](https://github.com/p4lang/p4c/pull/3493)]
- Fix incorrect CFG generated for BMV2 backend for switch statements [[view](https://github.com/p4lang/p4c/pull/3490)]
- Fix bug related to varbits width [[view](https://github.com/p4lang/p4c/pull/3487)]
- Delete keys that are empty tuples [[view](https://github.com/p4lang/p4c/pull/3489)]
- Support GCC-style options for writing dependencies to a file. [[view](https://github.com/p4lang/p4c/pull/3484)]
- Support inverted ranges [[view](https://github.com/p4lang/p4c/pull/3482)]
- Improve generation of unique strings [[view](https://github.com/p4lang/p4c/pull/3350)]
- missing return
- Dpdk Backend: Allow non-constant arguments to mirror_packet extern [[view](https://github.com/p4lang/p4c/pull/3473)]
- Log/IndentCtl sanity/cleanup
- readme: Update supported Ubuntu versions [[view](https://github.com/p4lang/p4c/pull/3476)]
- Fix ir_frontend_midend_control_plane build target [[view](https://github.com/p4lang/p4c/pull/3475)]
- Remove GMP dependency from BUILD.bazel [[view](https://github.com/p4lang/p4c/pull/3474)]
- Dpdk backend: optional and range match kinds translate to wildcard match for dpdk [[view](https://github.com/p4lang/p4c/pull/3472)]

## Release v1.2.3.0 [[view](https://github.com/p4lang/p4c/pull/3466)]
- Remove "frontends" from the include directories. [[view](https://github.com/p4lang/p4c/pull/3467)]
- Implicit casts between InfInt and Bit are allowed [[view](https://github.com/p4lang/p4c/pull/3469)]
- Dpdk Backend: Move Mux handling after simplifying select expression [[view](https://github.com/p4lang/p4c/pull/3464)]
- Remove undefined behaviour in pass manager initialization [[view](https://github.com/p4lang/p4c/pull/3468)]
- Add a compiler pass that adds missing IDs to control plane objects. [[view](https://github.com/p4lang/p4c/pull/3455)]
- Forbid packages, controls, etc as tuple fields [[view](https://github.com/p4lang/p4c/pull/3461)]
- Do not allow constructor calls of externs with abstract methods [[view](https://github.com/p4lang/p4c/pull/3460)]
- Fix include headers for lib/stringify.h [[view](https://github.com/p4lang/p4c/pull/3457)]
- Use Type_Dontcare for _ expressions in table entries [[view](https://github.com/p4lang/p4c/pull/3444)]
- pft-ebpf tests: update psabpf to fix tests [[view](https://github.com/p4lang/p4c/pull/3456)]
- Fix Missing metadata for temporary variables added for Mask operation [[view](https://github.com/p4lang/p4c/pull/3452)]
- DPDK Backend: Add support for tdi.json [[view](https://github.com/p4lang/p4c/pull/3440)]
- P4C-DPDK - Incorrect constant value when there is a substract operation involved with a constant [[view](https://github.com/p4lang/p4c/pull/3411)]
- Fixed build with older bison on Ubuntu 18 [[view](https://github.com/p4lang/p4c/pull/3454)]
- Add bazel support for p4c-dpdk compiler [[view](https://github.com/p4lang/p4c/pull/3441)]
- initial commit [[view](https://github.com/p4lang/p4c/pull/3447)]
- Values with type int are legal constants [[view](https://github.com/p4lang/p4c/pull/3446)]
- Fixed usage of yyFlexLexer with multiple parsers [[view](https://github.com/p4lang/p4c/pull/3445)]
- Prevent crash caused by incorrect error message [[view](https://github.com/p4lang/p4c/pull/3443)]
- ordered_set: operator<: Fix for a case of strict inclusion. [[view](https://github.com/p4lang/p4c/pull/3431)]
- DPDK Backend: Avoid compiler crash by handling possible types of length argument in packet extract for header containing varbit field [[view](https://github.com/p4lang/p4c/pull/3439)]
- DPDK Backend: Emit name information for externs in context json [[view](https://github.com/p4lang/p4c/pull/3433)]
- [dpdk] shorten label names and header instance type [[view](https://github.com/p4lang/p4c/pull/3436)]
- remove debug info [[view](https://github.com/p4lang/p4c/pull/3438)]
- Added new P4C option [[view](https://github.com/p4lang/p4c/pull/3425)]
- Avoid crash in ebpf backend by checking expected IR node kinds [[view](https://github.com/p4lang/p4c/pull/3427)]
- remove unused include [[view](https://github.com/p4lang/p4c/pull/3428)]
- Do not remove externs which are defined not in system files. [[view](https://github.com/p4lang/p4c/pull/3424)]
- Implement Copy Elimination for dpdk backend [[view](https://github.com/p4lang/p4c/pull/3406)]
- save annotations after parser unroll [[view](https://github.com/p4lang/p4c/pull/3422)]
- Handle constant folding for tuple/list constants [[view](https://github.com/p4lang/p4c/pull/3421)]
- loop unrolling: Use type of original expression [[view](https://github.com/p4lang/p4c/pull/3420)]

## Release v1.2.2.3 [[view](https://github.com/p4lang/p4c/pull/3418)]
- Inhibit compiler optimizations in GC malloc()] implementation [[view](https://github.com/p4lang/p4c/pull/3415)]
- Fix def-use analysis bug when handling virtual methods [[view](https://github.com/p4lang/p4c/pull/3419)]
- Only crash if the error count exceeds the maximum [[view](https://github.com/p4lang/p4c/pull/3413)]
- Use a helper to to define the cpp toolchain type [[view](https://github.com/p4lang/p4c/pull/3416)]
- Fix incorrect overload resolution for specialized (package)] types [[view](https://github.com/p4lang/p4c/pull/3412)]
- Always prepend local gtest include paths [[view](https://github.com/p4lang/p4c/pull/3414)]
- Add support of path expressions for a header stack inside parserUnroll [[view](https://github.com/p4lang/p4c/pull/3405)]
- Values with type int are legal constants [[view](https://github.com/p4lang/p4c/pull/3408)]
- collects decls from parser and control block [[view](https://github.com/p4lang/p4c/pull/3392)]
- Update BUILD.bazel to remove constat patterns from glob [[view](https://github.com/p4lang/p4c/pull/3397)]
- Remove apply()] calls to redundant parsers [[view](https://github.com/p4lang/p4c/pull/3356)]
- psabpf-ctl: update action-selector CLI - rename commands [[view](https://github.com/p4lang/p4c/pull/3404)]
- Fix incorrect copy propagation into table match keys [[view](https://github.com/p4lang/p4c/pull/3399)]
- Solving problems with HeaderUnion [[view](https://github.com/p4lang/p4c/pull/3342)]
- Update rules that require a cpp toolchain to use a helper function that defines the toolchain type. [[view](https://github.com/p4lang/p4c/pull/3401)]
- Improve the way we wait for commands in driver [[view](https://github.com/p4lang/p4c/pull/3398)]
- Fixing parserUnroll bug for sequential next operator usage [[view](https://github.com/p4lang/p4c/pull/3193)]
- Fix missing key initialization when header/metadata fields used as table keys are copied in new metadata fields [[view](https://github.com/p4lang/p4c/pull/3395)]
- actionSynthesis: add original source info to synthesized actions [[view](https://github.com/p4lang/p4c/pull/3396)]
- Use width_bits()] instead of size for constantFolding. [[view](https://github.com/p4lang/p4c/pull/3400)]
- BUILD_USE_COLOR=OFF disables colors for the C++ compiler [[view](https://github.com/p4lang/p4c/pull/3389)]
- Make bitvec helpers available as global, don't polute top-level namespace [[view](https://github.com/p4lang/p4c/pull/3393)]
- Bring commonly used builtin options under lib directory [[view](https://github.com/p4lang/p4c/pull/3381)]
- Update reference outputs [[view](https://github.com/p4lang/p4c/pull/3388)]
- Dpdk Backend: Enable header stack dynamic index elimination [[view](https://github.com/p4lang/p4c/pull/3375)]
- Add metadata fields uses as Member Expression [[view](https://github.com/p4lang/p4c/pull/3387)]
- Enable Automated Releases [[view](https://github.com/p4lang/p4c/pull/3386)]
- DPDK Backend: Fix action name used in learn instruction, minor fix in context json [[view](https://github.com/p4lang/p4c/pull/3385)]
- Fixing bug in HSIndexSimplifier path [[view](https://github.com/p4lang/p4c/pull/3382)]
- PSA-eBPF PTF tests: update psabpf to the latest version [[view](https://github.com/p4lang/p4c/pull/3383)]
- Dpdk backend: Add target_name field for tables to context json.  [[view](https://github.com/p4lang/p4c/pull/3380)]
- Fixed comparison between two Type_Bits [[view](https://github.com/p4lang/p4c/pull/3377)]
- accept PSA_MeterColor_t in action parameter [[view](https://github.com/p4lang/p4c/pull/3365)]
- Type unification should always use the canonical type [[view](https://github.com/p4lang/p4c/pull/3370)]
- Dpdk backend: Minor changes in context json and learner table timeout values. Supercedes PR #3369 [[view](https://github.com/p4lang/p4c/pull/3372)]
- P4C-DPDK : Add support for Hash extern [[view](https://github.com/p4lang/p4c/pull/3368)]
- Add types for tables keys after switchElimination [[view](https://github.com/p4lang/p4c/pull/3064)]
- Do not allow variables with type Type_Parser/Control [[view](https://github.com/p4lang/p4c/pull/3361)]
- Remove unused variables in psa/ebpf backend [[view](https://github.com/p4lang/p4c/pull/3362)]
- String utility functions [[view](https://github.com/p4lang/p4c/pull/3366)]
- crash.cpp: Register names fixed for building under Cygwin. [[view](https://github.com/p4lang/p4c/pull/3296)]
- Added nullptr check into graphs backend [[view](https://github.com/p4lang/p4c/pull/3367)]
- Add old parser state in loopsUnroll if ExpressionEvaluator returns a error [[view](https://github.com/p4lang/p4c/pull/3150)]
- Use 0 as default PSA_PORT_RECIRCULATE value [[view](https://github.com/p4lang/p4c/pull/3360)]
- Add support for ternary const entries [[view](https://github.com/p4lang/p4c/pull/3347)]
- Improve error message when select types do not match [[view](https://github.com/p4lang/p4c/pull/3352)]
- Do not crash if constant width is 0 [[view](https://github.com/p4lang/p4c/pull/3358)]
- Allow value sets as labels when select()] is done on a constant [[view](https://github.com/p4lang/p4c/pull/3353)]
- Allow unification between serenum and underlying type [[view](https://github.com/p4lang/p4c/pull/3341)]
- Allow shifts with serenums on the RHS [[view](https://github.com/p4lang/p4c/pull/3355)]
- Unification of generic types can reduce to unification of Type_Name [[view](https://github.com/p4lang/p4c/pull/3354)]
- Fixed start state renaming for p4-14 translation using @packet_entry [[view](https://github.com/p4lang/p4c/pull/3348)]
- Validate value_set element types [[view](https://github.com/p4lang/p4c/pull/3351)]
- Do not generate TC Egress program if PSA Egress pipeline is empty [[view](https://github.com/p4lang/p4c/pull/3349)]
- Improve error message when validating select expressions [[view](https://github.com/p4lang/p4c/pull/3340)]
- Require a start state in parsers [[view](https://github.com/p4lang/p4c/pull/3339)]
- Use psabpf-ctl register command in PTF tests [[view](https://github.com/p4lang/p4c/pull/3337)]
- Dpdk:  Fix missing initialization for table properties for add_on_miss tables in context json [[view](https://github.com/p4lang/p4c/pull/3334)]
- Do not specialize functions when arguments are type variables [[view](https://github.com/p4lang/p4c/pull/3328)]
- Add generic structs to P4Runtime digest example [[view](https://github.com/p4lang/p4c/pull/3332)]
- Fix control-plane generation for headers containing structs [[view](https://github.com/p4lang/p4c/pull/3331)]
- Check that parameter default values are compile-time known [[view](https://github.com/p4lang/p4c/pull/3330)]
- Fix handling of keys without name annotation in graphs [[view](https://github.com/p4lang/p4c/pull/3327)]
- [dpdk]  Fix calculation of direction from input port [[view](https://github.com/p4lang/p4c/pull/3326)]
- DPDK Backend: Connection tracking support [[view](https://github.com/p4lang/p4c/pull/3290)]
- Update PSA-eBPF documentation [[view](https://github.com/p4lang/p4c/pull/3320)]
- Expand 'emit' calls in the ebpf backend [[view](https://github.com/p4lang/p4c/pull/3323)]
- Fix warnings generated by XDP helper program [[view](https://github.com/p4lang/p4c/pull/3322)]
- Fixes for Graph generation for switch(t.apply()].action_run)] [[view](https://github.com/p4lang/p4c/pull/3314)]
- Allow shifts with constant signed positive amounts [[view](https://github.com/p4lang/p4c/pull/3303)]
- Correctly handle don't care named arguments  [[view](https://github.com/p4lang/p4c/pull/3278)]
- Avoid dereferencing nullptr in GetStackItemRef::baseRef [[view](https://github.com/p4lang/p4c/pull/3315)]
- Not all returns in parsers are disallowed [[view](https://github.com/p4lang/p4c/pull/3309)]
- Do not allow structure have specialized parsers as type arguments [[view](https://github.com/p4lang/p4c/pull/3310)]
- Fixing bug for concat elimination in SimpleSwitch [[view](https://github.com/p4lang/p4c/pull/3300)]
- Require generic return types to be specialized [[view](https://github.com/p4lang/p4c/pull/3302)]
- p4test does not support switch statements in actions [[view](https://github.com/p4lang/p4c/pull/3301)]
- Forbid instantiations in functions or methods [[view](https://github.com/p4lang/p4c/pull/3279)]
- Forbid methods to return int or string [[view](https://github.com/p4lang/p4c/pull/3280)]
- Emit table apply for switch expression with action_run [[view](https://github.com/p4lang/p4c/pull/3284)]
- Test for isValid as a header field [[view](https://github.com/p4lang/p4c/pull/3275)]
- Improvement of p4c-graphs backend [[view](https://github.com/p4lang/p4c/pull/3268)]
- Add support for LLD linker, cmake options for LLD and Gold [[view](https://github.com/p4lang/p4c/pull/3263)]
- Update used psabpf library for PTF tests (PSA/eBPF)] [[view](https://github.com/p4lang/p4c/pull/3261)]
- Preserve build failure semantics. [[view](https://github.com/p4lang/p4c/pull/3270)]
- [dpdk] Eliminate header copy [[view](https://github.com/p4lang/p4c/pull/3257)]
- DPDK Backend: PNA Pass metadata support [[view](https://github.com/p4lang/p4c/pull/3245)]
- Add UPF sample program for PSA/eBPF backend [[view](https://github.com/p4lang/p4c/pull/3267)]
- Add BNG sample program for PSA/eBPF backend [[view](https://github.com/p4lang/p4c/pull/3266)]
- Use ccache with Github actions [[view](https://github.com/p4lang/p4c/pull/3073)]
- Add cmake option to force-enable colors in build [[view](https://github.com/p4lang/p4c/pull/3269)]
- Do not crash on illegal uses of 'this' [[view](https://github.com/p4lang/p4c/pull/3265)]
- Allow specialized types for direct control/parser invocations [[view](https://github.com/p4lang/p4c/pull/3260)]
- Add support for ternary match kind to eBPF backend [[view](https://github.com/p4lang/p4c/pull/3256)]
- Stricter type checking for built-in methods [[view](https://github.com/p4lang/p4c/pull/3259)]
- Add L2L3-ACL sample program for PSA/eBPF backend [[view](https://github.com/p4lang/p4c/pull/3255)]
- Improve padding calculation in eBPF parser [[view](https://github.com/p4lang/p4c/pull/3254)]
- UBPF backend header files were not cpplinted [[view](https://github.com/p4lang/p4c/pull/3252)]
- Fix incorrect C code generation for ebpf control plane [[view](https://github.com/p4lang/p4c/pull/3253)]
- Add support for Random extern to PSA/eBPF backend [[view](https://github.com/p4lang/p4c/pull/3251)]
- Fix support for verify()] and error type to fully support verify()] in PSA/eBPF backend [[view](https://github.com/p4lang/p4c/pull/3250)]
- Implement DirectMeter extern for eBPF backend [[view](https://github.com/p4lang/p4c/pull/3243)]
- p4c: Handle spaces in file name [[view](https://github.com/p4lang/p4c/pull/3249)]
- Fix a typo in README [[view](https://github.com/p4lang/p4c/pull/3248)]

## Release 1.2.2.2 [[view](https://github.com/p4lang/p4c/pull/3247)]
- Point to PSA/eBPF implementation in main README [[view](https://github.com/p4lang/p4c/pull/3244)]
- Add support for parser value_set to eBPF backend [[view](https://github.com/p4lang/p4c/pull/3235)]
- Fixed bitvec.h [[view](https://github.com/p4lang/p4c/pull/3239)]
- Fix error message for cast to 'type' [[view](https://github.com/p4lang/p4c/pull/3241)]
- manage lifetimes of Visitor::visited [[view](https://github.com/p4lang/p4c/pull/3168)]
- Allow P4 filenames containing spaces [[view](https://github.com/p4lang/p4c/pull/3242)]
- DPDK Backend: Add support for recirculate()] extern [[view](https://github.com/p4lang/p4c/pull/3236)]
- Do not allow casts to struct types [[view](https://github.com/p4lang/p4c/pull/3234)]
- P4C-DPDK - Support Non-zero arguments for default action [[view](https://github.com/p4lang/p4c/pull/3206)]
- Implement Meter extern for eBPF backend [[view](https://github.com/p4lang/p4c/pull/3231)]
- Replace pna direction with register read [[view](https://github.com/p4lang/p4c/pull/3224)]
- Strength reduction should not remove some casts [[view](https://github.com/p4lang/p4c/pull/3226)]
- Try to cancel previous Jenkins runs [[view](https://github.com/p4lang/p4c/pull/3230)]
- Fix expansion of signed ranges to masks [[view](https://github.com/p4lang/p4c/pull/3212)]
- improving the driver re: the checking of input pathnames, improving error messages, trying to prevent misleading error messages. [[view](https://github.com/p4lang/p4c/pull/3218)]
- Allow casts int to int [[view](https://github.com/p4lang/p4c/pull/3220)]
- Add support for Hash extern to eBPF backend [[view](https://github.com/p4lang/p4c/pull/3215)]
- Add support for ActionSelector extern to PSA/eBPF backend [[view](https://github.com/p4lang/p4c/pull/3216)]
- P4C-DPDK: Emit error when struct fields >64-bit is present in the P4 [[view](https://github.com/p4lang/p4c/pull/3217)]
- DPDK Backend: Fix binary operations with 1st operand is not same as dst operand [[view](https://github.com/p4lang/p4c/pull/3209)]
- Solving problems with Header Union verify function (reopened)] [[view](https://github.com/p4lang/p4c/pull/3214)]
- Add support for Register extern to PSA/eBPF backend [[view](https://github.com/p4lang/p4c/pull/3202)]
- Fix Context json to emit target_action_name and NoAction [[view](https://github.com/p4lang/p4c/pull/3213)]
- [dpdk] Emit key name in context file as in bfrt and p4info [[view](https://github.com/p4lang/p4c/pull/3205)]
- Add support for DirectCounter extern to PSA/eBPF backend [[view](https://github.com/p4lang/p4c/pull/3195)]
- Insert type specializations before functions [[view](https://github.com/p4lang/p4c/pull/3207)]
- Remove incorrect test for recursion [[view](https://github.com/p4lang/p4c/pull/3199)]
- Add support for InternetChecksum extern to eBPF backend [[view](https://github.com/p4lang/p4c/pull/3194)]
- adding tests re the driver (1)] missing error messages and (2)] giving misleading error messages [[view](https://github.com/p4lang/p4c/pull/3198)]
- [dpdk] Shorten the Identifer name, including dots(.)] in Member expression [[view](https://github.com/p4lang/p4c/pull/3175)]
- Add --xdp2tc compiler flag for eBPF backend [[view](https://github.com/p4lang/p4c/pull/3187)]
- test: Update install_fedora_deps.sh [[view](https://github.com/p4lang/p4c/pull/3190)]
- Add support for ActionProfile extern to PSA/eBPF backend [[view](https://github.com/p4lang/p4c/pull/3177)]
- Constant fold casts of serializble enums [[view](https://github.com/p4lang/p4c/pull/3181)]
- Improve error messages during type checking [[view](https://github.com/p4lang/p4c/pull/3182)]
- Add support for Checksum extern and CRC16/32 algorithms to eBPF backend [[view](https://github.com/p4lang/p4c/pull/3167)]
- Add Digest extern to PSA/eBPF backend [[view](https://github.com/p4lang/p4c/pull/3164)]
- typedef with generic types requires type arguments [[view](https://github.com/p4lang/p4c/pull/3174)]
- Add support for Counter extern to PSA/eBPF backend [[view](https://github.com/p4lang/p4c/pull/3165)]
- Handle struct expression in ebfp backend [[view](https://github.com/p4lang/p4c/pull/3173)]
- Fixed underflow during row-access in the SymBitMatrix [[view](https://github.com/p4lang/p4c/pull/3143)] [[view](https://github.com/p4lang/p4c/pull/3169)]
- PSA-eBPF: Add missing header name substitution in deparser [[view](https://github.com/p4lang/p4c/pull/3162)]
- DPDK Backend: Insert table keys generated by compiler closer to the standard and user metadata [[view](https://github.com/p4lang/p4c/pull/3160)]
- DPDK Backend: Move learn instructions constant argument to metadata [[view](https://github.com/p4lang/p4c/pull/3163)]
- Removed execute permission bits from all 3 of the P4 files that had them.
- Add PTF test infra for eBPF backend [[view](https://github.com/p4lang/p4c/pull/3158)]
- Implement PSA for eBPF backend [[view](https://github.com/p4lang/p4c/pull/3139)]
- Represent unary plus in the IR [[view](https://github.com/p4lang/p4c/pull/3157)]
- Disable 'unused' warning for some psa.p4 externs [[view](https://github.com/p4lang/p4c/pull/3147)]
- P4C-DPDK-Support header field with Slice as a Table key [[view](https://github.com/p4lang/p4c/pull/3151)]
- typeChecker: make error message less confusing [[view](https://github.com/p4lang/p4c/pull/3152)]
- Compile the P4 compiler tools in host configuration [[view](https://github.com/p4lang/p4c/pull/3148)]
- Keep track of source position for p4-14 switch statement labels [[view](https://github.com/p4lang/p4c/pull/3141)]
- Split psa_switch.h into two files [[view](https://github.com/p4lang/p4c/pull/3144)]
- Improve efficiency of range translation to mask  [[view](https://github.com/p4lang/p4c/pull/3133)]
- Added nullptr checks for l/rtype [[view](https://github.com/p4lang/p4c/pull/3138)]
- Implement EBPF deparser [[view](https://github.com/p4lang/p4c/pull/3136)]
- Add packet_in.length()] and packet_in.advance()] methods to a eBPF target [[view](https://github.com/p4lang/p4c/pull/3137)]
- sfinae-protect operator== StringRef overload [[view](https://github.com/p4lang/p4c/pull/3132)]
- DPDK: Implementation for non byte aligned metadata and header fields [[view](https://github.com/p4lang/p4c/pull/3114)]
- Comparisons of list expressions, structure-valued expressions, tuples and structs #3057 [[view](https://github.com/p4lang/p4c/pull/3122)]
- DPDK: Add support for "mirror_packet" PNA extern [[view](https://github.com/p4lang/p4c/pull/3128)]
- Type of a unary expression is not always the same as the input type [[view](https://github.com/p4lang/p4c/pull/3135)]
- Enable generation of pointer variables by eBPF codeGen [[view](https://github.com/p4lang/p4c/pull/3131)]
- Eliminate typedefs in bmv2-ss backend [[view](https://github.com/p4lang/p4c/pull/3123)]
- Struct expressions can be ConstantExpressions [[view](https://github.com/p4lang/p4c/pull/3125)]
- Solution for "Non Type_Bits type bool for expression" error [[view](https://github.com/p4lang/p4c/pull/3120)]
- Use 0 as action ID for NoAction and refactor eBPF table implementation [[view](https://github.com/p4lang/p4c/pull/3129)]
- Extend eBPF kernel target with support for additional BPF helpers and more types of BPF maps [[view](https://github.com/p4lang/p4c/pull/3119)]
- Fix off-by-one bug in source file position calculation [[view](https://github.com/p4lang/p4c/pull/3124)]
- from1.0: fix register type signess [[view](https://github.com/p4lang/p4c/pull/3117)]
- Print the current node type name when checkedTo fails [[view](https://github.com/p4lang/p4c/pull/3116)]
- DPDK: Fix core dump while generating bfrt json for PNA programs with action selector [[view](https://github.com/p4lang/p4c/pull/3115)]
- Fix cpplint errors for dpdk sources [[view](https://github.com/p4lang/p4c/pull/3111)]
- DPDK: Fix missing action definitions in spec file [[view](https://github.com/p4lang/p4c/pull/3109)]
- Refactor SetupJoinPoints to make it more extensible
- Flatten local struct decl in dpdk [[view](https://github.com/p4lang/p4c/pull/3108)]
- Remove unnecessary check in conversion of log_msg to a JSON [[view](https://github.com/p4lang/p4c/pull/3067)]
- loop_revisit method for dealing with recursive loops in the IR [[view](https://github.com/p4lang/p4c/pull/3106)]
- Forbid egress pipeline in dpdk by default [[view](https://github.com/p4lang/p4c/pull/3104)]
- Improve error message for count()] in dpdk [[view](https://github.com/p4lang/p4c/pull/3101)]
- Handle bfrt Info for new type [[view](https://github.com/p4lang/p4c/pull/3103)]
- Add dpdk version string [[view](https://github.com/p4lang/p4c/pull/3105)]
- Add debug messages for generated eBPF programs [[view](https://github.com/p4lang/p4c/pull/3097)]
- Add correct and error test cases for structure-values expressions [[view](https://github.com/p4lang/p4c/pull/3100)]
- Treat match_kind as a regular type [[view](https://github.com/p4lang/p4c/pull/3099)]
- dpdk: Eliminate unused metadata fields [[view](https://github.com/p4lang/p4c/pull/3096)]
- Preserve some types in constant folding. [[view](https://github.com/p4lang/p4c/pull/3094)]
- Emit semicolon after method call [[view](https://github.com/p4lang/p4c/pull/3093)]
- Dismantle Mux expression in dpdk [[view](https://github.com/p4lang/p4c/pull/3087)]
- Moved FindRecirculated from .cpp to .h [[view](https://github.com/p4lang/p4c/pull/3088)]
