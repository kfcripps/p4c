#include <gtest/gtest.h>

#include <optional>

#include "frontends/p4/frontend.h"
#include "frontends/p4/dontcareArgs.h"
#include "frontends/p4/toP4/toP4.h"
#include "helpers.h"
#include "ir/dump.h"
#include "ir/ir.h"
#include "lib/sourceCodeBuilder.h"

using namespace P4;

namespace Test {

namespace {

std::optional<FrontendTestCase> createDontCareArgsTestCase() {
    std::string source = P4_SOURCE(P4Headers::V1MODEL, R"(
struct S {
    bit<64> f;
}

control C(inout S s) {
    @name("d") action d_0(@name("b") out bit<64> b_0) {
        b_0 = 64w4;
    }
    @name("foo") action foo_0() {
        d_0(_);
    }
    @name("t") table t_0 {
        actions = {
            foo_0();
        }
        default_action = foo_0();
    }
    apply {
        t_0.apply();
    }
}

control proto(inout S s);
package top(proto p);
top(C()) main;
    )");

    return FrontendTestCase::create(source, CompilerOptions::FrontendVersion::P4_16);
}

}  // namespace

class RemoveDontcareArgsTest : public P4CTest {};

TEST_F(RemoveDontcareArgsTest, Default) {
    ReferenceMap refMap;
    TypeMap typeMap;

    auto test = createDontCareArgsTestCase();

    Util::SourceCodeBuilder builder;
    ToP4 top4(builder, false);
    test->program->apply(top4);

    std::string program_string = builder.toString();

    std::string expected_source = P4_SOURCE(P4Headers::V1MODEL, R"(
struct S {
    bit<64> f;
}

control C(inout S s) {
    @name("d") action d_0(@name("b") out bit<64> b_0) {
        b_0 = 64w4;
    }
    @name("foo") action foo_0() {
        bit<64> arg;
        d_0(arg);
    }
    @name("t") table t_0 {
        actions = {
            foo_0();
        }
        default_action = foo_0();
    }
    apply {
        t_0.apply();
    }
}

control proto(inout S s);
package top(proto p);
top(C()) main;
    )");

    EXPECT_TRUE(program_string == expected_source);
}

}  // namespace Test
