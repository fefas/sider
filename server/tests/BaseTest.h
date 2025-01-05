#ifndef SIDER_TESTS_BASE_TEST_H
#define SIDER_TESTS_BASE_TEST_H

#pragma once

#include <cstdint>
#include <gtest/gtest.h>

#include "../src/Clock.h"
#include "../src/Command/Command.h"
#include "../src/Registry/Registry.h"

namespace Sider::Tests
{
    using namespace Sider;

    struct FixedClock : public Clock::Clock
    {
        uint32_t fixedNow = 0;

        uint32_t now() override { return fixedNow; }
        void advance(uint32_t seconds) { fixedNow += seconds; }
    };

    struct TtlTestCase
    {
        uint32_t advanceTime;
        Command::Result expectedResult;
    };

    struct GetTestCase
    {
        Command::Command* getCommand;
        Command::Result expectedResult;
    };

    class BaseTest : public ::testing::Test
    {
        private:
        FixedClock* clock;
        Registry::Registry* registry;

        protected:
        void SetUp() override
        {
            setUpRegistry();
            setUpClock();
        }

        void TearDown() override
        {
            tearDownClock();
            tearDownRegistry();
        }

        Command::Result execute(Command::Command* command)
        {
            return command->execute(registry);
        }

        void advanceClock(uint32_t seconds)
        {
            clock->advance(seconds);
        }

        void assertEquals(Command::Result expected, Command::Result actual)
        {
            ASSERT_EQ(actual.success, expected.success);
            ASSERT_EQ(actual.value, expected.value);
        }

        private:
        void setUpRegistry()
        {
            registry = Registry::createEmpty();
        }

        void tearDownRegistry()
        {
            delete registry;
        }

        void setUpClock()
        {
            clock = new FixedClock();
            Registry::Register::setClock(clock);
        }

        void tearDownClock()
        {
            delete clock;
        }
    };
}

#endif
