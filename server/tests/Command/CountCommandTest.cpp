#include <gtest/gtest.h>

#include "CommandTest.h"

namespace Sider::Command
{
    class CountCommandTest : public CommandTest {};

    class GetCounterValueByScopeAndKey : public CountCommandTest, public testing::WithParamInterface<CommandResultScenario> {};
    INSTANTIATE_TEST_SUITE_P(CountCommandTest, GetCounterValueByScopeAndKey, testing::Values(
        CommandResultScenario{get("scope1", "key1"), Result::with(8)},
        CommandResultScenario{get("scope1", "key2"), Result::with(12)},
        CommandResultScenario{get("scope2", "key1"), Result::with(60)}
    ));

    TEST_P(GetCounterValueByScopeAndKey, CheckCounterValues)
    {
        auto [getCommand, expectedResult] = GetParam();

        execute(4, count("scope1", "key1", 2));
        execute(2, count("scope1", "key2", 6));
        execute(3, count("scope2", "key1", 20));

        assertCommandResult(expectedResult, getCommand);
    }

    TEST_F(CountCommandTest, GetNilIfTtlOfAllCountsIsExpired)
    {
        execute(count("ttl", "key", 1, 10));
        execute(count("ttl", "key", 1, 20));
        execute(count("ttl", "key", 1, 99));
        advanceTime(100);

        assertCommandResult(Result::nil(), get("ttl", "key"));
    }

    class CounterDecreasesWhileTtlCountsExpire : public CountCommandTest, public testing::WithParamInterface<AdcanceTimeScenario> {};
    INSTANTIATE_TEST_SUITE_P(CountCommandTest, CounterDecreasesWhileTtlCountsExpire, testing::Values(
        AdcanceTimeScenario{5,   Result::with(7)}, // After 5s: No item expired
        AdcanceTimeScenario{15,  Result::with(6)}, // After 15s: 10s TTL expired
        AdcanceTimeScenario{40,  Result::with(5)}, // After 40s: 20s TTL expired
        AdcanceTimeScenario{60,  Result::with(4)}, // After 60s: 50s TTL expired
        AdcanceTimeScenario{100, Result::with(4)}  // After 100s: 100s TTL not yet expired
    ));

    TEST_P(CounterDecreasesWhileTtlCountsExpire, GetCountAfterTimeAdvance)
    {
        auto [advance, expectedResult] = GetParam();

        execute(count("ttl", "key", 1, 10));
        execute(count("ttl", "key", 1, 20));
        execute(count("ttl", "key", 1, 50));
        execute(count("ttl", "key", 1, 100));
        execute(count("ttl", "key", 1, 100));
        execute(count("ttl", "key", 1, 100));
        execute(count("ttl", "key", 1, 100));
        advanceTime(advance);

        assertCommandResult(expectedResult, get("ttl", "key"));
    }
}
