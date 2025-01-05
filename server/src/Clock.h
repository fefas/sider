#ifndef SIDER_REGISTRY_REGISTER_CLOCK_H
#define SIDER_REGISTRY_REGISTER_CLOCK_H

#include <cstdint>

namespace Sider::Clock
{
    struct Clock
    {
        virtual uint32_t now() = 0;
    };

    // struct Time
    // {
    //     virtual bool isPassed(Time that) = 0;
    // }

    Clock* getClock();
    void setClock(Clock* clock);
}

#endif
