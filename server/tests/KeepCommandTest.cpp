#include <gtest/gtest.h>

#include "../src/Command/Command.h"
#include "BaseTest.h"

namespace Sider::Tests
{
    using namespace Sider;
    using namespace std;

    class KeepCommandTest : public BaseTest
    {
    protected:
        void SetUp() override
        {
            BaseTest::SetUp();
        }
    };

    class GetKeptValueByScopeAndKey : 
        public KeepCommandTest,
        public testing::WithParamInterface<GetTestCase>
    {};

    INSTANTIATE_TEST_SUITE_P(KeptValues, GetKeptValueByScopeAndKey, testing::Values(
        GetTestCase{Command::get("scope1", "key1"), Command::Result::with("value1")},
        GetTestCase{Command::get("scope1", "key2"), Command::Result::with("value2")}, 
        GetTestCase{Command::get("scope2", "key1"), Command::Result::with("value3")},
        GetTestCase{Command::get("scope2", "key2"), Command::Result::with("value4")}
    ));

    TEST_P(GetKeptValueByScopeAndKey, CheckValue)
    {
        auto [getCommand, expectedResult] = GetParam();
        execute(Command::keep("scope1", "key1", "value1"));
        execute(Command::keep("scope1", "key2", "value2"));
        execute(Command::keep("scope2", "key1", "value3"));
        execute(Command::keep("scope2", "key2", "value4"));

        Command::Result actualResult = execute(getCommand);
        
        assertEquals(expectedResult, actualResult);
    }

    TEST_F(KeepCommandTest, GetLatestKeptValueByScopeAndKey)
    {
        execute(Command::keep("scope", "key", "value1"));
        execute(Command::keep("scope", "key", "value2"));
        execute(Command::keep("scope", "key", "value3"));

        Command::Result actualResult = execute(Command::get("scope", "key"));

        assertEquals(Command::Result::with("value3"), actualResult);
    }

    TEST_F(KeepCommandTest, GetNilIfTtlIsExpired)
    {
        execute(Command::keep("scope", "key", "value", 100));
        advanceClock(101);

        Command::Result actualResult = execute(Command::get("scope", "key"));

        assertEquals(Command::Result::nil(), actualResult);
    }
}