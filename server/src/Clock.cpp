#include <cstdint>
#include <ctime>

#include "Clock.h"

namespace Sider::Clock
{
    static Clock* globalClock = nullptr;

    struct CClock : public Clock
    {
        uint32_t now() override
        {
            return time(nullptr);
        }
    };

    Clock* getClock()
    {
        if (!globalClock) {
            globalClock = new CClock();
        }

        return globalClock;
    }

    void setClock(Clock* clock)
    {
        globalClock = clock;
    }
}