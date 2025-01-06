#include <gtest/gtest.h>

#include "../../src/Command/Command.h"
#include "Test.h"

namespace Sider::Command
{
    class CountCommandTest : public Test {};

    TEST_F(CountCommandTest, GetNullIfNoRegisterIsFoundByScopeAndKey)
    {
        assertCommandResult(get("nscope", "nkey"), Result::nil());
    }

    class GetCounterValueByScopeAndKey : public CountCommandTest, public testing::WithParamInterface<CommandResultScenario> {};
    INSTANTIATE_TEST_SUITE_P(CountCommandTest, GetCounterValueByScopeAndKey, testing::Values(
        CommandResultScenario{get("scope1", "key1"), Result::with(4)},
        CommandResultScenario{get("scope1", "key2"), Result::with(2)},
        CommandResultScenario{get("scope2", "key1"), Result::with(3)}
    ));

    TEST_P(GetCounterValueByScopeAndKey, CheckCounterValues)
    {
        auto [getCommand, expectedResult] = GetParam();

        execute(4, count("scope1", "key1"));
        execute(2, count("scope1", "key2"));
        execute(3, count("scope2", "key1"));

        assertCommandResult(getCommand, expectedResult);
    }

    TEST_F(CountCommandTest, GetNilIfTtlOfAllCountsIsExpired)
    {
        execute(3, count("ttl", "key", 100));
        advanceClock(101);

        assertCommandResult(get("scope", "key"), Result::nil());
    }

    class CounterResultDecreasesWhenTtlCountsExpire : public CountCommandTest, public testing::WithParamInterface<TtlScenario> {};
    INSTANTIATE_TEST_SUITE_P(CountCommandTest, CounterResultDecreasesWhenTtlCountsExpire, testing::Values(
        TtlScenario{5,   Result::with(7)}, // After 5s: No item expired
        TtlScenario{15,  Result::with(6)}, // After 20s: 10s TTL expired
        TtlScenario{40,  Result::with(5)}, // After 40s: 20s TTL expired
        TtlScenario{60,  Result::with(4)}, // After 60s: 50s TTL expired
        TtlScenario{100, Result::with(4)}  // After 100s: 100s TTL not yet expired
    ));

    TEST_P(CounterResultDecreasesWhenTtlCountsExpire, GetCountAfterTimeAdvance)
    {
        auto [advanceTime, expectedResult] = GetParam();

        execute(count("ttl", "key", 10));
        execute(count("ttl", "key", 20));
        execute(count("ttl", "key", 50));
        execute(count("ttl", "key", 100));
        execute(count("ttl", "key", 100));
        execute(count("ttl", "key", 100));
        execute(count("ttl", "key", 100));
        advanceClock(advanceTime);

        assertCommandResult(get("ttl", "key"), expectedResult);
    }
}