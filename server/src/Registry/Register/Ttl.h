#ifndef SIDER_REGISTRY_REGISTER_TTL_H
#define SIDER_REGISTRY_REGISTER_TTL_H

#include <cstdint>
#include "../../Clock.h"

namespace Sider::Registry::Register
{
    using namespace Sider::Clock;

    struct Ttl
    {
        const uint32_t seconds;

        static Ttl* inSeconds(uint32_t seconds)
        {
            return seconds != 0 ? new Ttl(seconds) : nullptr;
        }
    };

    struct Expiration
    {
        const uint32_t at;

        static Expiration* withTtl(const Ttl* ttl)
        {
            return new Expiration(getClock()->now() + ttl->seconds);
        }

        bool isPassed() const
        {
            return getClock()->now() > at;
        }

        bool isEarlierThan(const Expiration* that) const
        {
            return at < that->at;
        }
    };

    struct TtlItems
    {
        virtual ~TtlItems() = 0;

        virtual bool isExpired() = 0;
        virtual void add(const Ttl* ttl) = 0;
        virtual uint32_t currentValue() = 0;
    };

    TtlItems* createTtlItems();
}

#endif
