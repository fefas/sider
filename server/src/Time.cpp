#include <cstdint>
#include <ctime>

#include "Time.h"

namespace Sider::Time
{
    uint32_t now()
    {
        return time(nullptr);
    }
}

