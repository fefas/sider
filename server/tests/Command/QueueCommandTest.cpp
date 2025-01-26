#include <gtest/gtest.h>

#include "../../src/Command/Command.h"
#include "CommandTest.h"

namespace Sider::Command
{
    class QueueCommandTest : public CommandTest {};

    TEST_F(QueueCommandTest, GetFirstQueuedValue)
    {
        execute(queue("scope", "key", "value1"));
        execute(queue("scope", "key", "value2"));
        execute(queue("scope", "key", "value3"));

        assertCommandResult(Result::with("value1"), get("scope", "key"));
    }

    TEST_F(QueueCommandTest, GetNextQueuedValueAfterDequeue)
    {
        execute(queue("scope", "key", "value1"));
        execute(queue("scope", "key", "value2"));
        execute(queue("scope", "key", "value3"));

        execute(dequeue("scope", "key"));

        assertCommandResult(Result::with("value2"), get("scope", "key"));
    }

    TEST_F(QueueCommandTest, GetNilAfterAllQueuedValuesAreDequeued)
    {
        execute(queue("scope", "key", "value1"));
        execute(queue("scope", "key", "value2"));
        execute(queue("scope", "key", "value3"));

        execute(dequeue("scope", "key"));
        execute(dequeue("scope", "key"));
        execute(dequeue("scope", "key"));

        assertCommandResult(Result::nil(), get("scope", "key"));
    }

    class GetvaluesOutOfManyQueues : public QueueCommandTest, public testing::WithParamInterface<CommandResultScenario> {};
    INSTANTIATE_TEST_SUITE_P(KeepCommandTest, GetvaluesOutOfManyQueues, testing::Values(
        CommandResultScenario{get("scope1", "key1"), Result::with("value1")},
        CommandResultScenario{get("scope1", "key2"), Result::with("value1")}, 
        CommandResultScenario{get("scope2", "key1"), Result::with("value3")},
        CommandResultScenario{get("scope2", "key2"), Result::nil()}
    ));

    TEST_P(GetvaluesOutOfManyQueues, CheckValue)
    {
        auto [getCommand, expectedResult] = GetParam();

        execute(queue("scope1", "key1", "value1"));
        execute(queue("scope1", "key2", "value1"));
        execute(queue("scope1", "key2", "value2"));
        execute(queue("scope2", "key1", "value1"));
        execute(queue("scope2", "key1", "value2"));
        execute(queue("scope2", "key1", "value3"));
        execute(dequeue("scope2", "key1"));
        execute(dequeue("scope2", "key1"));
        execute(queue("scope2", "key2", "value"));
        execute(dequeue("scope2", "key2"));

        assertCommandResult(expectedResult, getCommand);
    }

    TEST_F(QueueCommandTest, ClearQueuedValues)
    {
        execute(queue("scope", "key", "value"));
        execute(queue("scope", "key", "value"));
        execute(queue("scope", "key", "value"));
        execute(queue("scope", "key", "value"));

        execute(clear("scope", "key"));

        assertCommandResult(Result::nil(), get("scope", "key"));
    }
}