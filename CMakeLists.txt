# Copyright (C) 2024 Intel Corporation
# 
# Licensed under the Apache License, Version 2.0 (the "License"); you may not
# use this file except in compliance with the License.  You may obtain a copy
# of the License at
# 
# http://www.apache.org/licenses/LICENSE-2.0
# 
# Unless required by applicable law or agreed to in writing, software distributed
# under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
# CONDITIONS OF ANY KIND, either express or implied.  See the License for the
# specific language governing permissions and limitations under the License.
# 
# SPDX-License-Identifier: Apache-2.0

# This is the CMake version supplied by Ubuntu 20.04.
cmake_minimum_required (VERSION 3.16.3 FATAL_ERROR)

find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    MESSAGE(STATUS "Enabling ccache")
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
endif()

project (P4C)

# set (CMAKE_CXX_EXTENSIONS OFF) # prefer using -std=c++20 rather than -std=gnu++20
set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

set (CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
set (P4C_RUNTIME_OUTPUT_DIRECTORY bin)
set (P4C_LIBRARY_OUTPUT_DIRECTORY lib)
set (P4C_ARTIFACTS_OUTPUT_DIRECTORY share/p4c)
set (CMAKE_USE_RELATIVE_PATHS 1)

OPTION (ENABLE_DOCS "Build the documentation" OFF)
OPTION (ENABLE_GTESTS "Enable building and running GTest unit tests" ON)
OPTION (ENABLE_BMV2 "Build the BMV2 backend (required for the full test suite)" ON)
OPTION (ENABLE_EBPF "Build the EBPF backend (required for the full test suite)" ON)
OPTION (ENABLE_UBPF "Build the uBPF backend (required for the full test suite)" ON)
OPTION (ENABLE_DPDK "Build the DPDK backend (required for the full test suite)" ON)
OPTION (ENABLE_TOFINO "Build the TOFINO backend (required for the full test suite)" OFF)
OPTION (ENABLE_P4TC "Build the P4TC backend" ON)
OPTION (ENABLE_P4FMT "Build the P4FMT backend" ON)
OPTION (ENABLE_P4TEST "Build the P4Test backend (required for the full test suite)" ON)
OPTION (ENABLE_TEST_TOOLS "Build the P4Tools development platform" OFF)
OPTION (ENABLE_P4C_GRAPHS "Build the p4c-graphs backend" ON)
OPTION (ENABLE_PROTOBUF_STATIC "Link against Protobuf statically" ON)
OPTION (ENABLE_GC "Compile with the Boehm-Demers-Weiser garbage collector." ON)
OPTION (ENABLE_WERROR "Treat warnings as errors" OFF)
OPTION (ENABLE_SANITIZERS "Enable sanitizers" OFF)
OPTION (STATIC_BUILD_WITH_DYNAMIC_GLIBC "Build a (mostly) statically linked release binary. \
Glibc is linked dynamically. WARNING: This only works if all dependencies that depend on the C++ \
standard library can be linked statically, otherwise the build will likely be broken and it will \
likely depend on the standard libary anyway. If you have some non-static C++ dependencies, use \
STATIC_BUILD_WITH_DYNAMIC_STDLIB." OFF)
OPTION (STATIC_BUILD_WITH_DYNAMIC_STDLIB "Build a (mostly) statically linked release binary. \
Glibc and C++ standard library is linked dynamically." OFF)

if (STATIC_BUILD_WITH_DYNAMIC_GLIBC OR STATIC_BUILD_WITH_DYNAMIC_STDLIB)
    # We want to optimize the binary size for a static release binary by default.
    set (_ENABLE_LTO_DEFAULT ON)
else()
    set (_ENABLE_LTO_DEFAULT OFF)
endif()
OPTION (ENABLE_LTO "Enable Link Time Optimization (LTO)" ${_ENABLE_LTO_DEFAULT})

OPTION (BUILD_AUTO_VAR_INIT_PATTERN "Initialize variables with pattern during build" OFF)
OPTION (ENABLE_IWYU "Enable checking includes with IWYU" OFF)
# Support a legacy option. TODO: Remove?
OPTION (ENABLE_UNIFIED_COMPILATION "Enable CMAKE_UNITY_BUILD" OFF)

# Enable DumpPipe pass output
OPTION(ENABLE_DUMP_PIPE "Enable DumpPipe pass output" ON)

set (P4C_DRIVER_NAME "p4c" CACHE STRING "Customize the name of the driver script")

set(MAX_LOGGING_LEVEL 10 CACHE STRING "Control the maximum logging level for -T logs")
set_property(CACHE MAX_LOGGING_LEVEL PROPERTY STRINGS 0 1 2 3 4 5 6 7 8 9 10)

if (NOT CMAKE_BUILD_TYPE)
  set (CMAKE_BUILD_TYPE "Release")
endif()

if (NOT $ENV{P4C_VERSION} STREQUAL "")
  # Allow the version to be set from outside
  set (P4C_VERSION $ENV{P4C_VERSION})
else()
  # Semantic version numbering: <major>.<minor>.<patch>[-rcX]
  # Examples: 0.5.1, 1.0.0-rc1, 1.0.1-alpha
  execute_process (COMMAND cat Version.txt
    OUTPUT_VARIABLE P4C_SEM_VERSION_STRING
    OUTPUT_STRIP_TRAILING_WHITESPACE
    RESULT_VARIABLE rc
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
  string (REGEX MATCH "([0-9]+)\\.([0-9]+)\\.([0-9]+)([-0-9a-z\\.]*).*"
    __p4c_version ${P4C_SEM_VERSION_STRING})
  set (P4C_VERSION_MAJOR ${CMAKE_MATCH_1})
  set (P4C_VERSION_MINOR ${CMAKE_MATCH_2})
  set (P4C_VERSION_PATCH ${CMAKE_MATCH_3})
  set (P4C_VERSION_RC ${CMAKE_MATCH_4})
  execute_process (COMMAND git rev-parse --short HEAD
    OUTPUT_VARIABLE P4C_GIT_SHA
    OUTPUT_STRIP_TRAILING_WHITESPACE
    RESULT_VARIABLE rc
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR})
  set (P4C_VERSION "${P4C_SEM_VERSION_STRING} (SHA: ${P4C_GIT_SHA} BUILD: ${CMAKE_BUILD_TYPE})")
endif()

# P4 General Utilities
include(P4CUtils)
# CMake Utilities to fetch dependencies.
include(FetchContent)
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
# More utilities.
include(CheckIncludeFile)
include(CheckIncludeFileCXX)
include(CheckFunctionExists)
include(FindPythonModule)
include(CPack)
# TODO: Remove this deprecated include eventually.
include(UnifiedBuild)

# # search in /usr/local first
# set (CMAKE_FIND_ROOT_PATH "/usr/local/bin" "${CMAKE_FIND_ROOT_PATH}")
set (P4C_CXX_FLAGS "")
set (P4C_LIB_DEPS)


# Support the legacy unified build option.
if(ENABLE_UNIFIED_COMPILATION)
  message(
    WARNING
      "Using deprecated option \"ENABLE_UNIFIED_COMPILATION\". Please use \"CMAKE_UNITY_BUILD\" instead."
  )
  set(CMAKE_UNITY_BUILD ON)
endif()

# If unity builds are enabled, choose an aggressive batch size.
if (CMAKE_UNITY_BUILD)
  set(CMAKE_UNITY_BUILD_BATCH_SIZE 10 CACHE UNINITIALIZED "Set the unity build batch size.")
endif ()

# Set the required options for a (mostly) static release build.
if(STATIC_BUILD_WITH_DYNAMIC_GLIBC OR STATIC_BUILD_WITH_DYNAMIC_STDLIB)
  if (STATIC_BUILD_WITH_DYNAMIC_GLIBC AND STATIC_BUILD_WITH_DYNAMIC_STDLIB)
    message(WARNING "Both STATIC_BUILD_WITH_DYNAMIC_GLIBC and STATIC_BUILD_WITH_DYNAMIC_STDLIB "
      "enabled at once; using 'DYNAMIC_STDLIB' settings (dynamic glibc and c++ standard library).")
  endif()
  message(STATUS "Building static release binaries")
  if (NOT STATIC_BUILD_WITH_DYNAMIC_STDLIB)
    message(WARNING "glibc is linked dynamically in current static builds.")
  endif()
  set(BUILD_SHARED_LIBS OFF)
  set(CMAKE_FIND_LIBRARY_SUFFIXES .a)
  # Link Boost statically
  # See https://cmake.org/cmake/help/latest/module/FindBoost.html for details
  set(Boost_USE_STATIC_LIBS ON)
  set(Boost_USE_STATIC_RUNTIME OFF)
  # Set the static variable
  set(P4C_STATIC_BUILD STATIC)
  # TODO: We can not use -static here because of compilation and portability problems:
  # TODO: Keep track of https://stackoverflow.com/questions/3430400/linux-static-linking-is-dead
  # and see whether static linking improves on Linux.
  # Do not bring in dynamic libstdcc and libgcc
  set(CMAKE_EXE_LINKER_FLAGS
      "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc -Wl,-z,muldefs"
  )
  if (NOT STATIC_BUILD_WITH_DYNAMIC_STDLIB)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libstdc++")
  endif()
  add_definitions(-DP4C_STATIC_BUILD)
endif()

# Ensure we enable sanitizers before fetching dependencies so the correct flags
# are propagated below
if (ENABLE_SANITIZERS)
  append("-fsanitize=undefined,address" CMAKE_C_FLAGS CMAKE_CXX_FLAGS)
  # ubsan may generate 128-bit multiplication with explicit overflow check. This
  # is compiled down to _muloti4 libcall that is provided by compiler-rr but not
  # by libgcc
  # So, on Linux where clang uses libgcc by default we'd need to switch to
  # compiler-rt, though we'd pull unwinder from libgcc.
  # See https://bugs.llvm.org/show_bug.cgi?id=16404 for more information
  if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND UNIX AND NOT APPLE)
    append("--rtlib=compiler-rt --unwindlib=libgcc" CMAKE_EXE_LINKER_FLAGS)
  endif()
endif ()

if (BUILD_AUTO_VAR_INIT_PATTERN)
  add_cxx_compiler_option  ("-ftrivial-auto-var-init=pattern")
endif ()

# Enable DumpPipe pass output
if (ENABLE_DUMP_PIPE)
  add_definitions("-DENABLE_DUMP_PIPE=1")
endif ()

# Required tools and libraries.
find_package (PythonInterp 3 REQUIRED)
find_package (FLEX 2.0 REQUIRED)
find_package (BISON 3.0 REQUIRED)
# If we enable GTest, install GoogleTest with the appropriate options.
if (ENABLE_GTESTS)
  include(GoogleTest)
  p4c_obtain_googletest()
  # Some P4C source files include gtest files. This will be propagated to the
  # definition in config.h We need this definition to guard against compilation
  # errors.
  set(P4C_GTEST_ENABLED ON)
endif ()
include(Abseil)
p4c_obtain_abseil()
include(Protobuf)
p4c_obtain_protobuf()

# We require -pthread to make std::call_once work, even if we're not using threads...
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# The boost graph headers are optional and only required by the graphs back end.
find_package (Boost QUIET COMPONENTS graph)
if (Boost_FOUND)
  set (HAVE_LIBBOOST_GRAPH 1)
else ()
  message (WARNING "Boost graph headers not found, will not build 'graphs' backend")
endif ()
find_package (Boost REQUIRED COMPONENTS iostreams)

# Compile with the Boehm garbage collector (https://github.com/ivmai/bdwgc), if requested.
# One can disable the GC, e.g., to run under Valgrind, by editing config.h.
if (ENABLE_GC)
  include(BDWGC)
  p4c_obtain_bdwgc()
endif ()
if (ENABLE_MULTITHREAD)
  add_definitions(-DMULTITHREAD)
endif()
list (APPEND P4C_LIB_DEPS ${CMAKE_THREAD_LIBS_INIT})
include_directories(SYSTEM ${Boost_INCLUDE_DIRS})
set (HAVE_LIBBOOST_IOSTREAMS 1)
list (APPEND P4C_LIB_DEPS ${Boost_LIBRARIES})
if (ENABLE_GC)
  list (APPEND P4C_LIB_DEPS ${LIBGC_LIBRARIES})
endif ()
set (P4C_ABSL_LIBRARIES absl::flat_hash_map absl::flat_hash_set)
list (APPEND P4C_LIB_DEPS ${P4C_ABSL_LIBRARIES})

# Other required libraries.
p4c_add_library (rt clock_gettime HAVE_CLOCK_GETTIME)

# Check includes.
check_include_file(ucontext.h HAVE_UCONTEXT_H)
check_include_file(backtrace-supported.h HAVE_LIBBACKTRACE)
check_include_file_cxx(mm_malloc.h HAVE_MM_MALLOC_H)
check_include_file_cxx(cxxabi.h HAVE_CXXABI_H)
if (HAVE_LIBBACKTRACE)
  message(STATUS "Linking libbacktrace.")
  set (P4C_LIB_DEPS "${P4C_LIB_DEPS};-lbacktrace")
endif ()

# check functions

# set libraries to be used with check_function_exists
set (CMAKE_REQUIRED_LIBRARIES_PRECHECK ${CMAKE_REQUIRED_LIBRARIES})
if (ENABLE_GC)
  set (CMAKE_REQUIRED_LIBRARIES ${CMAKE_REQUIRED_LIBRARIES} ${LIBGC_LIBRARIES})
endif ()

check_function_exists (memchr HAVE_MEMCHR)
check_function_exists (pipe2 HAVE_PIPE2)

# restore CMAKE_REQUIRED_LIBRARIES
set (CMAKE_REQUIRED_LIBRARIES ${CMAKE_REQUIRED_LIBRARIES_PRECHECK})

# python modules
find_python_module (difflib REQUIRED)
find_python_module (shutil REQUIRED)
find_python_module (tempfile REQUIRED)
find_python_module (subprocess REQUIRED)
find_python_module (re REQUIRED)

# other packages
find_package (Doxygen)
find_package (BMV2)
# If we have found simple switch or psa switch or pna_nic, we also need scapy.
if(SIMPLE_SWITCH OR PSA_SWITCH OR PNA_NIC)
    find_python_module (scapy REQUIRED)
endif()
# enable CTest
enable_testing ()


# if we want to manage versions in CMake ...
# include (cmake/P4CVersion.cmake)
# set (CPACK_PACKAGE_VERSION_MAJOR ${__P4C_VERSION_MAJOR})
# set (CPACK_PACKAGE_VERSION_MINOR ${__P4C_VERSION_MINOR})
# set (CPACK_PACKAGE_VERSION_PATCH ${__P4C_VERSION_PATCH})
# if (__P4C_VERSION_RC)
#   set (CPACK_PACKAGE_VERSION_PATCH ${CPACK_PACKAGE_VERSION_PATCH}-${__P4C_VERSION_RC})
# endif ()

add_cxx_compiler_option ("-Wall")
add_cxx_compiler_option ("-Wextra")
add_cxx_compiler_option ("-Wno-overloaded-virtual")
add_cxx_compiler_option ("-Wno-deprecated")
add_cxx_compiler_option ("-Wno-deprecated-declarations")
# Make compiler follow the standard and not be too lenient. The increases the
# likelihood that compilation will not break with other compilers (mainly clang)
# or in future versions of GCC.
add_cxx_compiler_option ("-pedantic")

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
  # The ##__VA_ARGS__ GNU extension is needed for IR. But clang complains about it.
  # FIXME: with C++20 we would be able to use standard __VA_OPT__
  add_cxx_compiler_option ("-Wno-gnu-zero-variadic-macro-arguments")
endif()

if (ENABLE_WERROR)
  add_cxx_compiler_option  ("-Werror")
  if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    # GCC's -Waybe-uninitialized warnings are quite noisy and unlikely to catch
    # real issues - do not treat them as build errors.
    add_cxx_compiler_option ("-Wno-error=maybe-uninitialized")
  endif ()
endif ()

# If we're on GCC or Clang, use the prefer LLD or Gold linker if available.
set(BUILD_LINK_WITH_GOLD ON CACHE BOOL "Use Gold linker for build if available")
set(BUILD_LINK_WITH_LLD ON CACHE BOOL "Use LLD linker for build if available (overrides BUILD_LINK_WITH_GOLD)")

# Build with LTO
if (ENABLE_LTO AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  if (BUILD_LINK_WITH_LLD)
    message(WARNING "LLD does not work with GCC's LTO object format, switching to Gold.")
    set(BUILD_LINK_WITH_LLD OFF)
  endif ()
  add_cxx_compiler_option("-flto")
endif ()

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(_LD_USED "default system")
    set(_LD_OPT_USED "")
    macro(check_linker BUILD_LINK_WITH_VAR LINKER_OPT LINKER_NAME)
        if (${BUILD_LINK_WITH_VAR})
            execute_process(
                    COMMAND ${CMAKE_C_COMPILER} -fuse-ld=${LINKER_OPT} -Wl,--version
                    ERROR_QUIET OUTPUT_VARIABLE LD_VERSION)
            if ("${LD_VERSION}" MATCHES ${LINKER_NAME})
                set(_LD_USED "${LINKER_NAME}")
                set(_LD_OPT_USED "-fuse-ld=${LINKER_OPT}")
            endif()
        endif()
    endmacro()
    check_linker(BUILD_LINK_WITH_GOLD "gold" "GNU gold")
    check_linker(BUILD_LINK_WITH_LLD "lld" "LLD")
    append("${_LD_OPT_USED}" CMAKE_EXE_LINKER_FLAGS CMAKE_SHARED_LINKER_FLAGS CMAKE_MODULE_LINKER_FLAGS)
    message(STATUS "Using the ${_LD_USED} linker.")
    unset(LD_VERSION)
    unset(_LD_USED)
    unset(_LD_OPT_USED)
endif()

set(BUILD_USE_COLOR OFF CACHE BOOL "Use color in C++ compiler output (even if \
    the compiler does not detect terminal, e.g. when using ccache/distcc)")
if (BUILD_USE_COLOR)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fdiagnostics-color=always")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fdiagnostics-color=always")
    else()
        message(WARNING "Colors enabled (BUILD_USE_COLOR=ON) but we don't know "
                "how to enable them for ${CMAKE_CXX_COMPILER_ID} C++ compiler")
    endif()
else()
  if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
      set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fdiagnostics-color=never")
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fdiagnostics-color=never")
  endif()
endif()

include_directories (
  ${P4C_SOURCE_DIR}
  ${P4C_BINARY_DIR}
  ${P4C_SOURCE_DIR}/extensions
  )
set(CONFIG_PREFIX ${CMAKE_INSTALL_PREFIX})
set(CONFIG_PKGDATADIR ${CMAKE_INSTALL_PREFIX}/${P4C_ARTIFACTS_OUTPUT_DIRECTORY})

set (CMAKE_CXX_FLAGS         "${CMAKE_CXX_FLAGS} ${P4C_CXX_FLAGS}")
set (CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG} ${P4C_CXX_FLAGS}")
set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${P4C_CXX_FLAGS}")
set (CPPLINT_FILES)           # list to collect all files that need lint
set (TEST_TAGS "p4" CACHE INTERNAL "test tags") # list to collect all test tags
set (IR_DEF_FILES)           # list to collect all .def files

# Other configuration files that need to be generated
configure_file ("${P4C_SOURCE_DIR}/cmake/config.h.cmake" "${P4C_BINARY_DIR}/config.h")

# IR_GENERATOR_DIRECTORY is used to set the RUNTIME_OUTPUT_DIRECTORY property
# of the irgenerator target to the matching path
set (IR_GENERATOR_DIRECTORY ${P4C_BINARY_DIR}/tools/ir-generator)
set (IR_GENERATOR ${IR_GENERATOR_DIRECTORY}/irgenerator)

# Component libraries: must be defined before being used in the back end executables.
set (P4C_LIBRARIES controlplane midend frontend ir p4ctoolkit ir-generated)

# The order of adding subdirectories matters because of target dependencies.
add_subdirectory (tools/driver)
add_subdirectory (lib)
add_subdirectory (tools/ir-generator)
add_subdirectory (ir)


# add extensions - before the frontends as they produce IR and extra frontend sources
set(EXTENSION_IR_SOURCES)
# extra sources that need to be linked directly into p4test so that
# extensions can provide specific conversions (e.g., for externs)
set(EXTENSION_P4_14_CONV_SOURCES)

########################################## P4Runtime Begin #########################################
# TODO: Ideally, this code should be part of the control-plane CMakelists.txt file. However, because
# extensions which may depend on P4Runtime variables are instantiated before the control-plane we
# need to keep the P4Runtime installation in the top-level CMakelists file.
# Print download state while setting up P4Runtime.
set(FETCHCONTENT_QUIET_PREV ${FETCHCONTENT_QUIET})
set(FETCHCONTENT_QUIET OFF)
# Fetch and declare the P4Runtime library.
FetchContent_Declare(
  p4runtime
  GIT_REPOSITORY https://github.com/p4lang/p4runtime.git
  GIT_TAG        ec4eb5ef70dbcbcbf2f8357a4b2b8c2f218845a5
  GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(p4runtime)
set(FETCHCONTENT_QUIET ${FETCHCONTENT_QUIET_PREV})
message(STATUS "Done with setting up P4Runtime for P4C.")
# The standard P4Runtime protocol buffers message definitions live in the PI
# repo, which is included in this repo as a git module.
set(P4RUNTIME_STD_DIR ${p4runtime_SOURCE_DIR}/proto CACHE INTERNAL
                                                          "Path to the P4Runtime directory."
)
########################################## P4Runtime End ##########################################

add_subdirectory (frontends)
add_subdirectory (midend)
add_subdirectory (control-plane)

file (GLOB p4c_extensions RELATIVE ${P4C_SOURCE_DIR}/extensions ${P4C_SOURCE_DIR}/extensions/*)
MESSAGE ("-- Available extensions ${p4c_extensions}")
foreach (ext ${p4c_extensions})
  if (EXISTS ${P4C_SOURCE_DIR}/extensions/${ext}/CMakeLists.txt)
    # Generate an option that makes it possible to disable this extension.
    string(MAKE_C_IDENTIFIER ${ext} EXT_AS_IDENTIFIER)
    string(TOUPPER ${EXT_AS_IDENTIFIER} EXT_AS_OPTION_NAME)
    string(CONCAT ENABLE_EXT_OPTION "ENABLE_" ${EXT_AS_OPTION_NAME})
    string(CONCAT EXT_HELP_TEXT "Build the " ${ext} " backend")
    OPTION (${ENABLE_EXT_OPTION} ${EXT_HELP_TEXT} ON)

    if (${ENABLE_EXT_OPTION})
        add_subdirectory (extensions/${ext})
    endif()
  endif()
endforeach(ext)

# With the current implementation of ir-generator, all targets share the
# same ir-generated.h and ir-generated.cpp file, which means all targets
# share the same set of IR classes (frontend and backend). Backends such as the DPDK backend
# can introduce additional IR classes and cpp sources which need to be added to
# EXTENSION_IR_SOURCES.
add_subdirectory(backends/common)
if (ENABLE_BMV2)
    add_subdirectory (backends/bmv2)
endif ()
if (ENABLE_DPDK)
    add_subdirectory (backends/dpdk)
endif ()
if (ENABLE_EBPF)
    add_subdirectory (backends/ebpf)
endif ()
if (ENABLE_P4C_GRAPHS AND HAVE_LIBBOOST_GRAPH EQUAL 1)
  add_subdirectory (backends/graphs)
endif ()
if (ENABLE_P4TC)
    add_subdirectory (backends/tc)
endif ()
if (ENABLE_P4TEST)
    add_subdirectory (backends/p4test)
endif ()
if (ENABLE_TEST_TOOLS)
    add_subdirectory (backends/p4tools)
endif ()
if (ENABLE_P4FMT)
    add_subdirectory (backends/p4fmt)
endif ()
if (ENABLE_UBPF)
    add_subdirectory (backends/ubpf)
endif ()
if (ENABLE_TOFINO)
    add_subdirectory (backends/tofino)
endif ()
if (ENABLE_GTESTS)
  add_subdirectory (test)
endif ()

####################################### IR Generation Begin #######################################

set (IR_GENERATED_SRCS
  ${P4C_BINARY_DIR}/ir/ir-generated.h
  ${P4C_BINARY_DIR}/ir/ir-generated.cpp
  ${P4C_BINARY_DIR}/ir/gen-tree-macro.h)
set_source_files_properties(${IR_GENERATED_SRCS} PROPERTIES GENERATED TRUE)

# Fixup #line directives in the generated IR files
# This is a vestige of automake. CMake handles dependencies correctly,
# so we should output the line directives directly from the generator
#
# Moreover, we generate these files to a temporary location and update the
# build files only if they have changed. This avoids rebuilding the whole
# tree when small changes to the .def files affect only the .cpp file and
# not the headers.
set (fixup_file "${P4C_BINARY_DIR}/irgen-fixup.awk")
file(WRITE "${fixup_file}" "/^#\$/ { printf \"#line %d \\\"${P4C_BINARY_DIR}/ir/%s\\\"\\n\", NR+1, name; next; } 1\n")
set(temp_ir_genfiles
  ir/ir-generated.cpp.tmp ir/ir-generated.cpp.fixup
  ir/ir-generated.h.tmp   ir/ir-generated.h.fixup
  ir/gen-tree-macro.h.tmp ir/gen-tree-macro.h.fixup
)

add_custom_command (OUTPUT ${IR_GENERATED_SRCS}
  COMMAND ${IR_GENERATOR} -i ir/ir-generated.cpp.tmp -o ir/ir-generated.h.tmp -t ir/gen-tree-macro.h.tmp ${IR_DEF_FILES}
  COMMAND awk -v name=ir-generated.cpp -f ${fixup_file} ir/ir-generated.cpp.tmp > ir/ir-generated.cpp.fixup
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ir/ir-generated.cpp.fixup ir/ir-generated.cpp
  COMMAND awk -v name=ir-generated.h   -f ${fixup_file} ir/ir-generated.h.tmp > ir/ir-generated.h.fixup
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ir/ir-generated.h.fixup ir/ir-generated.h
  COMMAND awk -v name=gen-tree-macro.h -f ${fixup_file} ir/gen-tree-macro.h.tmp > ir/gen-tree-macro.h.fixup
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ir/gen-tree-macro.h.fixup ir/gen-tree-macro.h
  MAIN_DEPENDENCY ${IR_GENERATOR}
  DEPENDS irgenerator ${IR_DEF_FILES}
  COMMENT "Generating IR class files"
)

add_custom_target(genIR DEPENDS ${IR_GENERATED_SRCS})
set_source_files_properties(${IR_GENERATOR} PROPERTIES GENERATED TRUE)
add_library(ir-generated OBJECT ${IR_GENERATED_SRCS} ${EXTENSION_IR_SOURCES})
add_dependencies(ir-generated ir genIR)
target_link_libraries(ir-generated PUBLIC ir ${P4C_LIB_DEPS})


######################################## IR Generation End ########################################

# Header files
# p4test needs all the backend include files, whether the backend is enabled or not
# Note that we only provide the headers for the build env, they are only installed by the
# backend specific target.
set (OTHER_HEADERS
  backends/ebpf/p4include/ebpf_model.p4
  )
add_custom_target(update_includes ALL
  COMMAND ${CMAKE_COMMAND} -E make_directory ${P4C_BINARY_DIR}/p4include
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${P4C_SOURCE_DIR}/p4include/*.p4 ${P4C_BINARY_DIR}/p4include
  COMMAND ${CMAKE_COMMAND} -E make_directory ${P4C_BINARY_DIR}/p4include/bmv2
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${P4C_SOURCE_DIR}/p4include/bmv2/psa.p4 ${P4C_BINARY_DIR}/p4include/bmv2
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${P4C_SOURCE_DIR}/p4include/bmv2/pna.p4 ${P4C_BINARY_DIR}/p4include/bmv2
  COMMAND ${CMAKE_COMMAND} -E make_directory ${P4C_BINARY_DIR}/p4include/dpdk
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${P4C_SOURCE_DIR}/p4include/dpdk/psa.p4 ${P4C_BINARY_DIR}/p4include/dpdk
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${P4C_SOURCE_DIR}/p4include/dpdk/pna.p4 ${P4C_BINARY_DIR}/p4include/dpdk
  COMMAND for h in ${OTHER_HEADERS} \; do
    ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/\$$h ${P4C_BINARY_DIR}/p4include \;
  done
  )

# Installation
# Targets install themselves. Here we install the core headers
install (DIRECTORY ${P4C_SOURCE_DIR}/p4include
  DESTINATION ${P4C_ARTIFACTS_OUTPUT_DIRECTORY})

########################################## Linters Begin ##########################################
include(Linters)
########################################### Linters End ###########################################


# tags, etags
set (CTAGS_DIRS backends extensions frontends ir lib tools midend)
add_custom_target(tags
  COMMAND ctags -R --langmap=C++:+.def,Flex:+.l,YACC:+.ypp -I abstract=class -I interface=class ${CTAGS_DIRS}
  COMMAND cd tools/ir-generator && ctags -R --langmap=Flex:+.l,YACC:+.ypp . ../../lib
  WORKING_DIRECTORY ${P4C_SOURCE_DIR}
  COMMENT "Generating ctags")
add_custom_target(etags
  COMMAND ctags -e -R --langmap=C++:+.def,Flex:+.l,YACC:+.ypp -I abstract=class -I interface=class ${CTAGS_DIRS}
  COMMAND cd tools/ir-generator && ctags -e -R --langmap=Flex:+.l,YACC:+.ypp . ../../lib
  WORKING_DIRECTORY ${P4C_SOURCE_DIR}
  COMMENT "Generating extended ctags")

# check, recheck, check-*, check-ifail, gtest
p4c_get_nprocs(__parallel_test)
MESSAGE(STATUS "CTest parallel: -j ${__parallel_test}")
set (P4C_XFAIL_LOG ${CMAKE_CURRENT_BINARY_DIR}/Testing/Temporary/LastXfail.txt)
set (P4C_TEST_FLAGS -j "${__parallel_test}")

p4c_add_make_check(all)
list (REMOVE_DUPLICATES TEST_TAGS)
foreach(t ${TEST_TAGS})
  p4c_add_make_check(${t})
endforeach()

add_custom_target(check
  DEPENDS check-all)

add_custom_target(recheck
  DEPENDS recheck-all)

# uninstall target
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Uninstall.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)

# docs
if (ENABLE_DOCS AND DOXYGEN_FOUND)
  if(DOXYGEN_DOT_FOUND)
    set (HAVE_DOT 'YES')
  else()
    set (HAVE_DOT 'NO')
  endif()
  set (DOXYGEN_FILE ${P4C_SOURCE_DIR}/docs/doxygen/doxygen.cfg)
  add_custom_target(docs ALL
    COMMAND export SRCDIR="${P4C_SOURCE_DIR}" &&
            export PROJECT=${PROJECT_NAME} &&
            export HAVE_DOT=${HAVE_DOT} &&
            export DOT_PATH=${DOXYGEN_DOT_PATH} &&
            export GENERATE_HTML='YES' &&
            export GENERATE_PDF='NO' &&
            export DOCDIR=${P4C_BINARY_DIR}/doxygen_out &&
            ${DOXYGEN_EXECUTABLE} ${DOXYGEN_FILE}
    DEPENDS genIR
    COMMENT "Generating documentation")
  install (DIRECTORY ${P4C_BINARY_DIR}/doxygen-out/html/
    DESTINATION ${P4C_ARTIFACTS_OUTPUT_DIRECTORY}/docs)
endif()

# Packaging:
SET(CPACK_SOURCE_GENERATOR "TXZ")
SET(CPACK_SOURCE_PACKAGE_FILE_NAME
   "p4c-${P4C_SEM_VERSION_STRING}")
SET(CPACK_SOURCE_IGNORE_FILES
   "${PROJECT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}-*;${PROJECT_SOURCE_DIR}/${CMAKE_PROJECT_NAME}_*;/build/;/.git/;/config.log;/CMakeFiles/;CMakeCache.txt$;.tar.gz$;/_CPack_Packages;/Makefile$;~$;/build-deb;/clean-deb;/filter-empty-entries;/make-symbols;/make-ppa;/make-deb;/debian.conf;/make-rpm;/rpm.conf;${CPACK_SOURCE_IGNORE_FILES}")

ADD_CUSTOM_TARGET(dist COMMAND ${CMAKE_MAKE_PROGRAM} clean package_source)
