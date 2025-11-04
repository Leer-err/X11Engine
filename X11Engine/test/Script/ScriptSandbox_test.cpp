#include "ScriptSandbox.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <optional>

#include "lua.h"

using namespace Engine::Script;

TEST(ScriptSandboxTest, ScriptRunTest) {
    auto sandbox = ScriptSandbox();
    auto error = sandbox.runString("print(\"Hello World\")");

    EXPECT_EQ(error, std::nullopt);
}

TEST(ScriptSandboxTest, ScriptSyntaxErrorTest) {
    auto sandbox = ScriptSandbox();
    auto error = sandbox.runString("print(\"Hello World)");

    EXPECT_EQ(error, ScriptError::SyntaxError);
}

TEST(ScriptSandboxTest, StringTest) {
    auto sandbox = ScriptSandbox();
    sandbox.runString("a = \"Hello World\"");
    auto result = sandbox.getString("a");

    EXPECT_EQ(result.getResult(), "Hello World");
}

TEST(ScriptSandboxTest, NumberTest) {
    auto sandbox = ScriptSandbox();
    sandbox.runString("a = 42");
    auto result = sandbox.getNumber("a");

    EXPECT_EQ(result.getResult(), 42);
}

TEST(ScriptSandboxTest, WrongValueTypeTest) {
    auto sandbox = ScriptSandbox();
    sandbox.runString("a = \"Hello World\"");
    auto result = sandbox.getNumber("a");

    EXPECT_EQ(result.getError(), ValueError::WrongType);
}

TEST(ScriptSandboxTest, FunctionCallTest) {
    auto sandbox = ScriptSandbox();
    sandbox.runString("function foo() a = 1 end");
    auto error = sandbox.runFunction("foo");
    EXPECT_EQ(error, std::nullopt);

    auto value = sandbox.getNumber("a");
    EXPECT_EQ(value.getResult(), 1);
}

TEST(ScriptSandboxTest, ParametrizedCallTest) {
    auto sandbox = ScriptSandbox();
    sandbox.runString("function foo(a, b) c = a + b end");
    auto error = sandbox.runFunction("foo", 1, 2);
    EXPECT_EQ(error, std::nullopt);

    auto value = sandbox.getNumber("c");
    EXPECT_EQ(value.getResult(), 3);
}

TEST(ScriptSandboxTest, SandboxCopyTest) {
    auto sandbox = ScriptSandbox();
    sandbox.runString("a = \"Hello World\"");

    {
        auto sandbox_copy = sandbox;
        sandbox_copy.runString("a = \"Hello World2\"");
    }

    auto result = sandbox.getString("a");

    EXPECT_EQ(result.getResult(), "Hello World2");
}

TEST(ScriptSandboxTest, TableCreateTest) {
    auto sandbox = ScriptSandbox();
    sandbox.createTable("foo").addProperty("bar", 42).addProperty(
        "bar2", [](lua_State*) -> int { return 0; });
    sandbox.runString("a = foo.bar foo.bar2()");

    auto result = sandbox.getNumber("a");

    EXPECT_EQ(result.getResult(), 42);
}