#include <gtest/gtest.h>

#include "CommandTest.h"

namespace Sider::Command
{
    class TruncateCommandTest : public CommandTest {};

    class TruncateScopeScenario : public CommandTest, public testing::WithParamInterface<CommandResultScenario> {};
    INSTANTIATE_TEST_SUITE_P(TruncateCommandTest, TruncateScopeScenario, testing::Values(
        CommandResultScenario{get("scope1", "key1"), Result::nil()},
        CommandResultScenario{get("scope1", "key2"), Result::nil()},
        CommandResultScenario{get("scope2", "key1"), Result::with("value3")}
    ));

    TEST_P(TruncateScopeScenario, CheckEntriesAfterTruncate)
    {
        auto [getCommand, expectedResult] = GetParam();
        execute(keep("scope1", "key1", "value1"));
        execute(keep("scope1", "key2", "value2"));
        execute(keep("scope2", "key1", "value3"));

        execute(truncate("scope1"));

        assertCommandResult(expectedResult, getCommand);
    }

    TEST_F(TruncateCommandTest, DoNotTruncateByScopePrefix)
    {
        execute(keep("scope1", "key1", "value1"));

        execute(truncate("scope"));

        assertCommandResult(Result::with("value1"), get("scope1", "key1"));
    }

    TEST_F(TruncateCommandTest, TruncateNonExistingScope)
    {
        assertCommandResult(Result::ok(), truncate("scope1"));
    }
} 