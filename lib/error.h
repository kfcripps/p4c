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

#ifndef LIB_ERROR_H_
#define LIB_ERROR_H_

#include <type_traits>

#include "lib/compile_context.h"
#include "lib/cstring.h"
#include "lib/error_reporter.h"

// This should eventually be turned to 0 when all the code is converted
#define LEGACY 1

namespace P4 {

/// @return the number of errors encountered so far in the current compilation context.
inline unsigned errorCount() { return BaseCompileContext::get().errorReporter().getErrorCount(); }

/// @return the number of warnings encountered so far in the current compilation context.
inline unsigned warningCount() {
    return BaseCompileContext::get().errorReporter().getWarningCount();
}

/// @return the number of infos encountered so far in the current compilation context.
inline unsigned infoCount() { return BaseCompileContext::get().errorReporter().getInfoCount(); }

/// @return the number of diagnostics (either errors, warnings or infos) encountered so far in the
/// current compilation context.
inline unsigned diagnosticCount() {
    return BaseCompileContext::get().errorReporter().getDiagnosticCount();
}

// Errors, warnings, and infos are specified using boost::format format strings, i.e.,
// %1%, %2%, etc (starting at 1, not at 0).
// Some compatibility for printf-style arguments is also supported.

/// Report an error with the given message.
// LEGACY: once we transition to error types, this should be deprecated
#if LEGACY
template <typename... Args>
inline void error(const char *format, Args &&...args) {
    auto &context = BaseCompileContext::get();
    auto action = context.getDefaultErrorDiagnosticAction();
    context.errorReporter().diagnose(action, nullptr, format, "", std::forward<Args>(args)...);
}
#endif

/// Report errors of type kind. Requires that the node argument have source info.
/// The message format is declared in the error catalog.
template <class T, typename = std::enable_if_t<Util::has_SourceInfo_v<T>>, class... Args>
void error(const int kind, const char *format, const T *node, Args &&...args) {
    auto &context = BaseCompileContext::get();
    auto action = context.getDefaultErrorDiagnosticAction();
    context.errorReporter().diagnose(action, kind, format, "", node, std::forward<Args>(args)...);
}

/// This is similar to the above method, but also has a suffix
template <class T, typename = std::enable_if_t<Util::has_SourceInfo_v<T>>, class... Args>
void errorWithSuffix(const int kind, const char *format, const char *suffix, const T *node,
                     Args &&...args) {
    auto &context = BaseCompileContext::get();
    auto action = context.getDefaultErrorDiagnosticAction();
    context.errorReporter().diagnose(action, kind, format, suffix, node,
                                     std::forward<Args>(args)...);
}

/// The const ref variant of the above
template <class T, typename = std::enable_if_t<Util::has_SourceInfo_v<T> && !std::is_pointer_v<T>>,
          class... Args>
void error(const int kind, const char *format, const T &node, Args &&...args) {
    error(kind, format, &node, std::forward<Args>(args)...);
}

#if LEGACY
/// Convert errors that have a first argument as a node with source info to errors with kind
/// This allows incremental migration toward minimizing the number of errors and warnings
/// reported when passes are repeated, as typed errors are filtered.
// LEGACY: once we transition to error types, this should be deprecated
template <class T, typename = std::enable_if_t<Util::has_SourceInfo_v<T>>, class... Args>
void error(const char *format, const T *node, Args &&...args) {
    error(ErrorType::LEGACY_ERROR, format, node, std::forward<Args>(args)...);
}

/// The const ref variant of the above
// LEGACY: once we transition to error types, this should be deprecated
template <class T, typename = std::enable_if_t<Util::has_SourceInfo_v<T> && !std::is_pointer_v<T>>,
          class... Args>
void error(const char *format, const T &node, Args &&...args) {
    error(ErrorType::LEGACY_ERROR, format, node, std::forward<Args>(args)...);
}
#endif

/// Report errors of type kind for messages that do not have a node.
/// These will not be filtered
template <typename... Args>
void error(const int kind, const char *format, Args &&...args) {
    auto &context = BaseCompileContext::get();
    auto action = context.getDefaultErrorDiagnosticAction();
    context.errorReporter().diagnose(action, kind, format, "", std::forward<Args>(args)...);
}

/// Report an error if condition e is false.
#define ERROR_CHECK(e, ...)                 \
    do {                                    \
        if (!(e)) ::P4::error(__VA_ARGS__); \
    } while (0)

#if LEGACY
/// Report a warning with the given message.
template <typename... Args>
inline void warning(const char *format, Args &&...args) {
    auto &context = BaseCompileContext::get();
    auto action = context.getDefaultWarningDiagnosticAction();
    context.errorReporter().diagnose(action, nullptr, format, "", std::forward<Args>(args)...);
}
#endif

/// Report warnings of type kind. Requires that the node argument have source info.
template <class T, typename = std::enable_if_t<Util::has_SourceInfo_v<T>>, class... Args>
void warning(const int kind, const char *format, const T *node, Args &&...args) {
    auto &context = BaseCompileContext::get();
    auto action = context.getDefaultWarningDiagnosticAction();
    context.errorReporter().diagnose(action, kind, format, "", node, std::forward<Args>(args)...);
}

/// The const ref variant of the above
template <class T, typename = std::enable_if_t<Util::has_SourceInfo_v<T> && !std::is_pointer_v<T>>,
          class... Args>
void warning(const int kind, const char *format, const T &node, Args &&...args) {
    ::P4::warning(kind, format, &node, std::forward<Args>(args)...);
}

/// Report warnings of type kind, for messages that do not have a node.
/// These will not be filtered
template <typename... Args>
void warning(const int kind, const char *format, Args &&...args) {
    auto &context = BaseCompileContext::get();
    auto action = context.getDefaultWarningDiagnosticAction();
    context.errorReporter().diagnose(action, kind, format, "", std::forward<Args>(args)...);
}

/// Report a warning if condition e is false.
#define WARN_CHECK(e, ...)                    \
    do {                                      \
        if (!(e)) ::P4::warning(__VA_ARGS__); \
    } while (0)

/// Report info messages of type kind. Requires that the node argument have source info.
template <class T, typename = std::enable_if_t<Util::has_SourceInfo_v<T>>, class... Args>
void info(const int kind, const char *format, const T *node, Args &&...args) {
    auto &context = BaseCompileContext::get();
    auto action = context.getDefaultInfoDiagnosticAction();
    context.errorReporter().diagnose(action, kind, format, "", node, std::forward<Args>(args)...);
}

/// The const ref variant of the above
template <class T, typename = std::enable_if_t<Util::has_SourceInfo_v<T> && !std::is_pointer_v<T>>,
          class... Args>
void info(const int kind, const char *format, const T &node, Args &&...args) {
    ::P4::info(kind, format, &node, std::forward<Args>(args)...);
}

/// Report info messages of type kind, for messages that do not have a node.
/// These will not be filtered
template <typename... Args>
void info(const int kind, const char *format, Args &&...args) {
    auto &context = BaseCompileContext::get();
    auto action = context.getDefaultInfoDiagnosticAction();
    context.errorReporter().diagnose(action, kind, format, "", std::forward<Args>(args)...);
}

}  // namespace P4

#endif /* LIB_ERROR_H_ */
