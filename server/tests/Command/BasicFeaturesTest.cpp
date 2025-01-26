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
}