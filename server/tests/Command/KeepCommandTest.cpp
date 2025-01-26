#include <gtest/gtest.h>

#include "../../src/Command/Command.h"
#include "CommandTest.h"

namespace Sider::Command
{
    class KeepCommandTest : public CommandTest {};

    class GetKeptValueByScopeAndKey : public KeepCommandTest, public testing::WithParamInterface<CommandResultScenario> {};
    INSTANTIATE_TEST_SUITE_P(KeepCommandTest, GetKeptValueByScopeAndKey, testing::Values(
        CommandResultScenario{get("scope1", "key1"), Result::with("value1")},
        CommandResultScenario{get("scope1", "key2"), Result::with("value2")}, 
        CommandResultScenario{get("scope2", "key1"), Result::with("value3")},
        CommandResultScenario{get("scope2", "key2"), Result::with("value4")}
    ));

    TEST_P(GetKeptValueByScopeAndKey, CheckValue)
    {
        auto [getCommand, expectedResult] = GetParam();

        execute(keep("scope1", "key1", "value1"));
        execute(keep("scope1", "key2", "value2"));
        execute(keep("scope2", "key1", "value3"));
        execute(keep("scope2", "key2", "value4"));

        assertCommandResult(expectedResult, getCommand);
    }

    TEST_F(KeepCommandTest, GetLatestKeptValueByScopeAndKey)
    {
        execute(keep("scope", "key", "value1"));
        execute(keep("scope", "key", "value2"));
        execute(keep("scope", "key", "value3"));

        assertCommandResult(Result::with("value3"), get("scope", "key"));
    }

    TEST_F(KeepCommandTest, GetKeptValueIfTtlIsNotExpired)
    {
        execute(keep("scope", "key", "value", 100));

        assertCommandResult(Result::with("value"), get("scope", "key"));
    }

    TEST_F(KeepCommandTest, GetNilIfTtlIsExpired)
    {
        execute(keep("scope", "key", "value", 100));
        advanceTime(101);

        assertCommandResult(Result::nil(), get("scope", "key"));
    }
}