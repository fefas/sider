#pragma once

#include <cstdint>
#include "AdvanceableTime.h"

namespace Sider::Time
{
    static uint32_t time = 0;

    uint32_t now()
    {
        return time;
    }

    void advance(uint32_t seconds)
    {
        time += seconds;
    }
}