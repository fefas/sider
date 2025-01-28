#include <gtest/gtest.h>

#include "CommandTest.h"

namespace Sider::Command
{
    class RateCommandTest : public CommandTest {};

    class GetRateValueByScopeAndKey : public RateCommandTest, public testing::WithParamInterface<CommandResultScenario> {};
    INSTANTIATE_TEST_SUITE_P(RateCommandTest, GetRateValueByScopeAndKey, testing::Values(
        CommandResultScenario{get("scope", "key", 0), Result::with(40)},
        CommandResultScenario{get("scope", "key", 1), Result::with(10)},
        CommandResultScenario{get("scope", "key", 2), Result::with(20)},
        CommandResultScenario{get("scope", "key", 3), Result::with(30)}
    ));

    TEST_P(GetRateValueByScopeAndKey, CheckRateValues)
    {
        auto [getCommand, expectedResult] = GetParam();

        execute(rate("scope", "key", 0, 6));
        execute(rate("scope", "key", 0, 10));
        execute(rate("scope", "key", 1, 2));
        execute(rate("scope", "key", 1, 2));
        execute(rate("scope", "key", 2, 3));
        execute(rate("scope", "key", 2, 5));
        execute(rate("scope", "key", 3, 9));
        execute(rate("scope", "key", 3, 3));

        assertCommandResult(expectedResult, getCommand);
    }

    TEST_F(RateCommandTest, GetRateValueAfterTtlIsExpired)
    {
        execute(rate("scope", "key", 1, 10, 20));
        execute(rate("scope", "key", 0, 20, 40));
        execute(rate("scope", "key", 1, 20, 40));
        advanceTime(30);

        assertCommandResult(Result::with(50), get("scope", "key", 0));
    }

    TEST_F(RateCommandTest, GetNilIfTtlIsExpired)
    {
        execute(rate("scope", "key", 0, 6, 100));
        advanceTime(101);

        assertCommandResult(Result::nil(), get("scope", "key", 0));
    }

    class RateChangesWhileTtlCountsExpire : public RateCommandTest, public testing::WithParamInterface<AdcanceTimeScenario> {};
    INSTANTIATE_TEST_SUITE_P(CountCommandTest, RateChangesWhileTtlCountsExpire, testing::Values(
        AdcanceTimeScenario{5,   Result::with(50)}, // After 5s: No item expired
        AdcanceTimeScenario{15,  Result::with(53)}, // After 15s: 10s TTL expired
        AdcanceTimeScenario{40,  Result::with(70)}, // After 40s: 20s TTL expired
        AdcanceTimeScenario{60,  Result::with(50)}, // After 60s: 50s TTL expired
        AdcanceTimeScenario{90,  Result::nil()}     // After 90s: all items expired
    ));

    TEST_P(RateChangesWhileTtlCountsExpire, GetRateAfterTimeAdvance)
    {
        auto [advance, expectedResult] = GetParam();

        execute(rate("ttl", "key", 0, 120, 10));
        execute(rate("ttl", "key", 1,  80, 10));
        execute(rate("ttl", "key", 0, 160, 20));
        execute(rate("ttl", "key", 1,  40, 20));
        execute(rate("ttl", "key", 0,  20, 50));
        execute(rate("ttl", "key", 1, 180, 50));
        execute(rate("ttl", "key", 0, 100, 80));
        execute(rate("ttl", "key", 1, 100, 80));
        advanceTime(advance);

        assertCommandResult(expectedResult, get("ttl", "key", 1));
    }

    TEST_F(RateCommandTest, ClearRateValue)
    {
        execute(rate("scope", "key", 0, 8));
        execute(clear("scope", "key"));

        assertCommandResult(Result::nil(), get("scope", "key", 0));
    }
} 