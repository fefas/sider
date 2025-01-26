#pragma once

#include <cstdint>
#include <gtest/gtest.h>

#include "AdvanceableTime.h"
#include "../../src/Command/Command.h"
#include "../../src/Storage/Storage.h"

namespace Sider::Command
{
    struct AdcanceTimeScenario
    {
        uint32_t advanceTime;
        Result expectedResult;
    };

    struct CommandResultScenario
    {
        Command* command;
        Result expectedResult;
    };

    class CommandTest : public ::testing::Test
    {
        private:
        Sider::Storage::Storage* storage;

        protected:
        void SetUp() override
        {
            storage = Sider::Storage::empty();
        }

        void TearDown() override
        {
            delete storage;
        }

        void advanceTime(uint32_t seconds)
        {
            Time::advance(seconds);
        }

        void assertEquals(Result expected, Result actual)
        {
            ASSERT_EQ(expected.value, actual.value);
            ASSERT_EQ(expected.success, actual.success);
        }

        void assertCommandResult(Result expected, Command* command)
        {
            assertEquals(expected, execute(command));
        }

        Result execute(Command* command)
        {
            return command->execute(storage);
        }

        void execute(uint16_t repeat, Command* command)
        {
            while (repeat--) {
                execute(command);
            }
        }
    };
}
