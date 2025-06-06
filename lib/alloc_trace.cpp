#include "alloc_trace.h"

#include "absl/debugging/symbolize.h"
#include "hex.h"
#include "log.h"
#include "n4.h"

#if HAVE_LIBBACKTRACE
#include <backtrace.h>

#include "exename.h"
#endif
#if HAVE_CXXABI_H
#include <cxxabi.h>
#endif

namespace P4 {

extern const char *addr2line(void *addr, const char *text);

#if HAVE_LIBBACKTRACE
extern struct backtrace_state *global_backtrace_state;

void bt_error(void *, const char *msg, int) { BUG("%s", msg); }

int bt_print_callback(void *out_, uintptr_t pc, const char *file, int line, const char *func) {
    std::ostream &out = *static_cast<std::ostream *>(out_);
    if (file) {
        if (const char *p = strstr(file, "/frontends/")) {
            file = p + 1;
        } else if (const char *p = strstr(file, "/backends/")) {
            file = p + 1;
        } else if (const char *p = strstr(file, "/extensions/")) {
            file = p + 1;
        } else if (const char *p = strstr(file, "/lib/")) {
            file = p + 1;
        } else if (const char *p = strstr(file, "/ir/")) {
            file = p + 1;
        } else if (const char *p = strstr(file, "/include/")) {
            file = p + 9;
        }
    } else {
        file = "??";
    }
    out << Log::endl << file << ":" << line;

    if (func) {
        char tmp[1024];
        if (absl::Symbolize((void *)pc, tmp, sizeof(tmp))) func = tmp;
        out << " (" << func << ")";
    }
    out << " [" << hex(pc) << "]";
    return 0;
}
#endif

void AllocTrace::count(void **bt, size_t sz) {
    backtrace tr(bt);
    data[tr][sz]++;
}

std::ostream &operator<<(std::ostream &out, const AllocTrace &at) {
#if HAVE_LIBGC
    PauseTrace temp_pause;
#endif
    using data_t = decltype(at.data)::value_type;
    std::vector<std::pair<size_t, const data_t *>> sorted;
    size_t total_total = 0;
    for (auto &e : at.data) {
        size_t total = 0;
        for (auto &al : e.second) total += al.first * al.second;
        sorted.emplace_back(total, &e);
        total_total += total;
    }
    std::sort(sorted.begin(), sorted.end(), [](auto &a, auto &b) { return a.first > b.first; });
#if HAVE_LIBBACKTRACE
    if (!global_backtrace_state) {
        // TODO: Do not use strdup here?
        auto executablePath = getExecutablePath();
        if (executablePath.empty()) {
            executablePath = "<unknown_path>";
        }
        const char *executableChar = strdup(executablePath.c_str());
        global_backtrace_state = backtrace_create_state(executableChar, 1, bt_error, &out);
    }
#endif

    out << "Allocated a total of " << n4(total_total) << "B memory";
    for (auto &s : sorted) {
        if (s.first < 1000000) break;  // ignore little stuff
        size_t count = 0;
        for (auto &al : s.second->second) count += al.second;
        out << Log::endl << "allocated " << n4(s.first) << "B in " << count << " calls";
        out << " from:" << Log::indent;

#if HAVE_LIBBACKTRACE
        for (int i = 1; i < ALLOC_TRACE_DEPTH; ++i) {
            /* due to calling the callback multiple times for inlined functions, we need to
             * do this in ascending order or it makes no sense */
            backtrace_pcinfo(global_backtrace_state,
                             reinterpret_cast<uintptr_t>(s.second->first.trace[i]),
                             bt_print_callback, bt_error, &out);
        }
#else
        char tmp[1024];
        for (int i = ALLOC_TRACE_DEPTH - 1; i >= 1; --i) {
            void *pc = s.second->first.trace[i];
            const char *symbol = "(unknown)";
            if (absl::Symbolize(pc, tmp, sizeof(tmp))) {
                symbol = tmp;
            }
            const char *alt = addr2line(pc, nullptr);
            out << "\n " << (alt ? alt : "??: ") << symbol << " [" << hex(pc) << "]";
        }
#endif
        out << Log::unindent;
    }
    return out;
}

}  // namespace P4
