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

#include "dump.h"

#include <set>

#include "ir/ir.h"
#include "ir/node.h"
#include "ir/visitor.h"
#include "lib/cstring.h"
#include "lib/gc.h"
#include "lib/indent.h"
#include "lib/source_file.h"

namespace P4 {

using namespace P4::literals;

namespace {
class IRDumper : public Inspector {
    std::ostream &out;
    unsigned maxdepth;
    cstring ignore;
    bool source;
    // @brief (maybe) output a one-line summary of a node (no newline or children)
    // @return false if the node should not be printed (nothing output)
    // @return true if the node is printed
    bool firstLine(const IR::Node *n) const {
        if (auto ctxt = getContext()) {
            if (unsigned(ctxt->depth) > maxdepth) return false;
            if (ctxt->parent && ctxt->parent->child_name && ignore == ctxt->parent->child_name)
                return false;
            out << indent_t(ctxt->depth);
            if (ctxt->child_name) out << ctxt->child_name << ": ";
        }
        out << "[" << n->id << "] ";
        if (source && n->srcInfo) out << "(" << n->srcInfo.toPositionString() << ") ";
        out << n->node_type_name();
        n->dump_fields(out);
        return true;
    }
    bool preorder(const IR::Node *n) override {
        if (!firstLine(n)) return false;
        out << std::endl;
        return true;
    }
    void revisit(const IR::Node *n) override {
        if (firstLine(n)) out << "..." << std::endl;
    }
    void loop_revisit(const IR::Node *n) override {
        if (firstLine(n)) out << "...(loop)" << std::endl;
    }
    bool preorder(const IR::Expression *e) override {
        if (!preorder(static_cast<const IR::Node *>(e))) return false;
        visit(e->type, "type");
        return true;
    }
    bool preorder(const IR::Constant *c) override {
        return preorder(static_cast<const IR::Node *>(c));
    }
    void postorder(const IR::Node *) override {
        if (getChildrenVisited() == 0) visitAgain();
    }

 public:
    IRDumper(std::ostream &o, unsigned m, cstring ign, bool src)
        : out(o), maxdepth(m), ignore(ign), source(src) {}
};
}  // namespace

void dump(std::ostream &out, const IR::Node *n, unsigned maxdepth) {
    n->apply(IRDumper(out, maxdepth, nullptr, false));
}
void dump(std::ostream &out, const IR::Node *n) { dump(out, n, ~0U); }
void dump(const IR::Node *n, unsigned maxdepth) { dump(std::cout, n, maxdepth); }
void dump(const IR::Node *n) { dump(n, ~0U); }
void dump(const IR::INode *n, unsigned maxdepth) { dump(std::cout, n->getNode(), maxdepth); }
void dump(const IR::INode *n) { dump(n, ~0U); }
void dump(uintptr_t p, unsigned maxdepth) { dump(reinterpret_cast<const IR::Node *>(p), maxdepth); }
void dump(uintptr_t p) { dump(p, ~0U); }

void dump_notype(const IR::Node *n, unsigned maxdepth) {
    n->apply(IRDumper(std::cout, maxdepth, "type"_cs, false));
}
void dump_notype(const IR::Node *n) { dump_notype(n, ~0U); }
void dump_notype(const IR::INode *n, unsigned maxdepth) {
    n->getNode()->apply(IRDumper(std::cout, maxdepth, "type"_cs, false));
}
void dump_notype(const IR::INode *n) { dump_notype(n, ~0U); }
void dump_notype(uintptr_t p, unsigned maxdepth) {
    dump_notype(reinterpret_cast<const IR::Node *>(p), maxdepth);
}
void dump_notype(uintptr_t p) { dump_notype(p, ~0U); }

void dump_src(const IR::Node *n, unsigned maxdepth) {
    n->apply(IRDumper(std::cout, maxdepth, "type"_cs, true));
}
void dump_src(const IR::Node *n) { dump_src(n, ~0U); }
void dump_src(const IR::INode *n, unsigned maxdepth) {
    n->getNode()->apply(IRDumper(std::cout, maxdepth, "type"_cs, true));
}
void dump_src(const IR::INode *n) { dump_src(n, ~0U); }

void dump(std::ostream &out, const Visitor::Context *ctxt) {
    if (!ctxt) return;
    dump(ctxt->parent);
    out << indent_t(ctxt->depth - 1);
    if (ctxt->parent) {
        if (ctxt->parent->child_name)
            out << ctxt->parent->child_name << ": ";
        else
            out << ctxt->parent->child_index << ": ";
    }
    if (ctxt->original != ctxt->node) {
        out << "<" << static_cast<const void *>(ctxt->original) << ":[" << ctxt->original->id
            << "] " << ctxt->original->node_type_name();
        ctxt->original->dump_fields(out);
        out << std::endl << indent_t(ctxt->depth - 1) << ">";
    }
    out << static_cast<const void *>(ctxt->node) << ":[" << ctxt->node->id << "] "
        << ctxt->node->node_type_name();
    ctxt->node->dump_fields(out);
    std::cout << std::endl;
}
void dump(const Visitor::Context *ctxt) { dump(std::cout, ctxt); }

std::string dumpToString(const IR::Node *n) {
    std::stringstream str;
    dump(str, n);
    return str.str();
}

bool DumpPipe::preorder(const IR::Node *pipe) {
#if ENABLE_DUMP_PIPE
    if (LOGGING(1)) {
        if (heading) {
            LOG1("-------------------------------------------------");
            LOG1(heading);
            LOG1("-------------------------------------------------");
            size_t maxMem = 0;
            size_t memUsed = gc_mem_inuse(&maxMem) / (1024 * 1024);
            maxMem = maxMem / (1024 * 1024);
            LOG1("*** mem in use = " << memUsed << "MB, heap size = " << maxMem << "MB");
        }
        if (LOGGING(2))
            dump(Log::Detail::fileLogOutput(__FILE__), pipe);
        else
            LOG1(*pipe);
    }
#endif  // ENABLE_DUMP_PIPE
    return false;
}

}  // namespace P4
