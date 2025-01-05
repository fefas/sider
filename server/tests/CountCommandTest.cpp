#include <gtest/gtest.h>

#include "../src/Command/Command.h"
#include "BaseTest.h"

namespace Sider::Tests
{
    using namespace Sider;

    class CountCommandTest : public BaseTest
    {
        protected:
        void SetUp() override
        {
            BaseTest::SetUp();

            execute(Command::count("scope1", "key1"));
            execute(Command::count("scope1", "key1"));
            execute(Command::count("scope1", "key1"));
            execute(Command::count("scope1", "key1"));

            execute(Command::count("scope1", "key2"));
            execute(Command::count("scope1", "key2"));

            execute(Command::count("scope2", "key1"));
            execute(Command::count("scope2", "key1"));
            execute(Command::count("scope2", "key1"));

        }
    };

    TEST_F(CountCommandTest, GetNullIfNoRegisterIsFoundByScopeAndKey)
    {
        Command::Result result = execute(Command::get("myScope", "key"));

        assertEquals(Command::Result::nil(), result);
    }

    TEST_F(CountCommandTest, GetCounterValueByScopeAndKey)
    {
        Command::Result result1 = execute(Command::get("scope1", "key1"));
        Command::Result result2 = execute(Command::get("scope1", "key2"));
        Command::Result result3 = execute(Command::get("scope2", "key1"));

        assertEquals(Command::Result::with(4), result1);
        assertEquals(Command::Result::with(2), result2);
        assertEquals(Command::Result::with(3), result3);
    }

    TEST_F(CountCommandTest, GetNilIfTtlIsExpired)
    {
        execute(Command::count("ttl", "key", 100));
        execute(Command::count("ttl", "key", 100));
        execute(Command::count("ttl", "key", 100));
        
        advanceClock(101);
        Command::Result actualResult = execute(Command::get("scope", "key"));

        assertEquals(Command::Result::nil(), actualResult);
    }

    class GetCounterValueByScopeAndKeyDiscartingExpiredOnes :
        public CountCommandTest,
        public testing::WithParamInterface<TtlTestCase>
    {};

    INSTANTIATE_TEST_SUITE_P(TtlTests, GetCounterValueByScopeAndKeyDiscartingExpiredOnes, testing::Values(
        TtlTestCase{5,   Command::Result::with(7)}, // After 5s: No item expired
        TtlTestCase{15,  Command::Result::with(6)}, // After 20s: All items remain
        TtlTestCase{40,  Command::Result::with(5)}, // After 40s: 10s TTL expired
        TtlTestCase{60,  Command::Result::with(4)}, // After 60s: 50s TTL expired
        TtlTestCase{100, Command::Result::with(4)}  // After 100s: 100s TTL not yet expired
    ));

    TEST_P(GetCounterValueByScopeAndKeyDiscartingExpiredOnes, CheckCountAfterTimeAdvance)
    {
        execute(Command::count("ttl", "key", 10));
        execute(Command::count("ttl", "key", 20));
        execute(Command::count("ttl", "key", 50));
        execute(Command::count("ttl", "key", 100));
        execute(Command::count("ttl", "key", 100));
        execute(Command::count("ttl", "key", 100));
        execute(Command::count("ttl", "key", 100));

        auto [advanceTime, expectedResult] = GetParam();
        
        advanceClock(advanceTime);
        Command::Result result = execute(Command::get("ttl", "key"));
        
        assertEquals(expectedResult, result);
    }
}