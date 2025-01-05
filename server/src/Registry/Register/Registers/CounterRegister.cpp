#include <cstdint>
#include <stdexcept>

#include "../Register.h"
#include "../Ttl.h"

namespace Sider::Registry::Register
{
    class CCounterRegister : public CounterRegister
    {
        private:
        uint32_t ttlLessCounter;
        TtlItems* ttlCounter; // todo do not use pointer

        public:
        CCounterRegister(const Id* id) :
            CounterRegister(id),
            ttlLessCounter(0),
            ttlCounter(createTtlItems())
        {}

        ~CCounterRegister() override
        {
            if (ttlCounter) {
                delete ttlCounter;
            }
        }

        void increment(const Ttl* ttl) override
        {
            if (ttl) ttlCounter->add(ttl); 
            else ttlLessCounter++;
        }

        const uint32_t get() override
        {
            return (const uint32_t) (ttlLessCounter + ttlCounter->currentValue());
        }

        bool isExpired() override
        {
            return ttlCounter->isExpired();
        }
    };

    CounterRegister* createCounterRegister(const Id* id)
    {
        return new CCounterRegister(id);
    }
}