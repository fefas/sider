#ifndef SIDER_COMMAND_TEST_H
#define SIDER_COMMAND_TEST_H

#include <cstdint>
#include <gtest/gtest.h>

#include "../../src/Clock.h"
#include "../../src/Command/Command.h"
#include "../../src/Registry/Registry.h"

namespace Sider::Command
{
    using namespace Sider;
    using namespace Sider::Clock;

    class ControllableClock : public Clock
    {
        uint32_t fixedNow = 0;

        public:
        uint32_t now() override { return fixedNow; }
        void advance(uint32_t seconds) { fixedNow += seconds; }
    };

    struct TtlScenario
    {
        uint32_t advanceTime;
        Result expectedResult;
    };

    struct CommandResultScenario
    {
        Command* command;
        Result expectedResult;
    };

    class Test : public ::testing::Test
    {
        private:
        ControllableClock* clock;
        Registry::Registry* registry;

        protected:
        void SetUp() override
        {
            registry = Registry::createEmpty();
            clock = new ControllableClock();

            setClock(clock);
        }

        void TearDown() override
        {
            delete registry;
            delete clock;
        }

        void advanceClock(uint32_t seconds)
        {
            clock->advance(seconds);
        }

        Result execute(Command* command)
        {
            return command->execute(registry);
        }

        void execute(uint16_t repeat, Command* command)
        {
            while (repeat--) {
                execute(command);
            }
        }

        void assertEquals(Result expected, Result actual)
        {
            ASSERT_EQ(actual.value, expected.value);
            ASSERT_EQ(actual.success, expected.success);
        }

        void assertCommandResult(Command* command, Result expected)
        {
            assertEquals(expected, execute(command));
        }
    };
}

#endif
