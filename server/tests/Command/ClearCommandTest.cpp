#include <gtest/gtest.h>

#include "CommandTest.h"

namespace Sider::Command
{
    class ClearCommandTest : public CommandTest {};

    TEST_F(ClearCommandTest, ClearScope)
    {
        execute(keep("scope", "keep-entry", "keep"));
        execute(count("scope", "counter-entry", 10));
        execute(queue("scope", "queue-entry", "queue"));
        execute(rate("scope", "rater-entry", 1, 10));
        execute(keep("scope-with-same-prefix", "key", "value"));

        execute(clear("scope"));

        assertCommandResult(Result::nil(), get("scope", "keep-entry"));
        assertCommandResult(Result::nil(), get("scope", "counter-entry"));
        assertCommandResult(Result::nil(), get("scope", "queue-entry"));
        assertCommandResult(Result::nil(), get("scope", "rater-entry"));
        assertCommandResult(Result::with("value"), get("scope-with-same-prefix", "key"));
    }

    TEST_F(ClearCommandTest, ClearNonExistingScopeOrEntry)
    {
        assertCommandResult(Result::ok(), clear("scope"));
        assertCommandResult(Result::ok(), clear("scope", "key"));
    }
} 