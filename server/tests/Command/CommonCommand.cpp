#include <gtest/gtest.h>

#include "CommandTest.h"

namespace Sider::Command
{
    class CommonCommandTest : public CommandTest {};

    TEST_F(CommonCommandTest, GetNilIfNoEntryIsFoundByScopeAndKey)
    {
        assertCommandResult(Result::nil(), get("nscope", "nkey"));
    }

    TEST_F(CommonCommandTest, GetErrorIfCommandDoesNotMatchPreviousOne)
    {
        execute(keep("scope", "key", "value"));

        assertCommandResult(Result::error("Entry 'scope:key' is of type 'KEEPER'"), count("scope", "key", 1));
    }
}