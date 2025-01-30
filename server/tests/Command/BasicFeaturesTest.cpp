#include <gtest/gtest.h>

#include "CommandTest.h"

namespace Sider::Command
{
    class BasicFeaturesTest : public CommandTest {};

    TEST_F(BasicFeaturesTest, GetNilIfNoEntryIsFoundByScopeAndKey)
    {
        assertCommandResult(Result::nil(), get("nscope", "nkey"));
    }

    TEST_F(BasicFeaturesTest, GetErrorIfCommandDoesNotMatchPreviousOne)
    {
        execute(keep("scope", "key", "value"));

        assertCommandResult(Result::error("Entry 'scope:key' is of type 'KEEPER'"), count("scope", "key", 1));
    }

    class TruncateScopeScenario : public CommandTest, public testing::WithParamInterface<CommandResultScenario> {};
    INSTANTIATE_TEST_SUITE_P(BasicFeaturesTest, TruncateScopeScenario, testing::Values(
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
}