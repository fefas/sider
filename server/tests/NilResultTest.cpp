#include <gtest/gtest.h>

#include "../src/Command/Command.h"
#include "BaseTest.h"

namespace Sider::Tests
{
    using namespace Sider;

    class NilResultTest : public BaseTest
    {
    };

    TEST_F(NilResultTest, GetNilIfNoRegisterIsFoundByScopeAndKey)
    {
        Command::Result result = execute(Command::get("nscope", "nkey"));

        assertEquals(Command::Result::nil(), result);
    }
}