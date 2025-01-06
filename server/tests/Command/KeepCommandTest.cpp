#include <gtest/gtest.h>

#include "../../src/Command/Command.h"
#include "Test.h"

namespace Sider::Command
{
    using namespace Sider;
    using namespace std;

    class KeepCommandTest : public Test {};

    TEST_F(KeepCommandTest, GetNullIfNoRegisterIsFoundByScopeAndKey)
    {
        assertCommandResult(get("nscope", "nkey"), Result::nil());
    }

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

        assertCommandResult(getCommand, expectedResult);
    }

    TEST_F(KeepCommandTest, GetLatestKeptValueByScopeAndKey)
    {
        execute(keep("scope", "key", "value1"));
        execute(keep("scope", "key", "value2"));
        execute(keep("scope", "key", "value3"));

        assertCommandResult(get("scope", "key"), Result::with("value3"));
    }

    TEST_F(KeepCommandTest, GetNilIfTtlIsExpired)
    {
        execute(keep("scope", "key", "value", 100));
        advanceClock(101);

        assertCommandResult(get("scope", "key"), Result::nil());
    }
}