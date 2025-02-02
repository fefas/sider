#include <iostream>
#include <cstdint>
#include <stdexcept>
#include "../Entry.h"

namespace Sider::Storage::Entry
{
    struct ExpiringCounts
    {
        private:
        const Ttl ttl;
        const uint32_t countStep;
        ExpiringCounts* next;
        ExpiringCounts* prev;

        public:
        ExpiringCounts() :
            ttl(Ttl::forever()),
            countStep(0)
        {
            next = prev = this;
        }

        void add(Ttl ttl, uint32_t countStep)
        {
            ExpiringCounts* newCount = new ExpiringCounts(ttl, countStep);
            ExpiringCounts* current = next;

            while (newCount->expiresBeforeThan(current)) {
                current = current->next;
            }

            newCount->next = current;
            newCount->prev = current->prev;
            newCount->next->prev = newCount;
            newCount->prev->next = newCount;
        }

        uint32_t clearExpired()
        {
            uint32_t expiredCount = 0;
            ExpiringCounts* current = prev;

            while (current->ttl.isExpired()) {
                expiredCount += current->countStep;
                current = current->prev;

                delete current->next;
            }

            current->next = this;
            prev = current;

            return expiredCount;
        }

        private:
        ExpiringCounts(Ttl ttl, uint32_t countStep) :
            ttl(ttl),
            countStep(countStep),
            next(nullptr),
            prev(nullptr)
        {}

        bool expiresBeforeThan(const ExpiringCounts* that) const
        {
            return ttl.expiresBeforeThan(that->ttl);
        }
    };

    class CCounterEntry : public CounterEntry
    {
        private:
        ExpiringCounts queue;
        uint32_t count;

        public:
        CCounterEntry() :
            queue(ExpiringCounts()),
            count(0)
        {}

        Type type() const override
        {
            return Type::COUNTER;
        }

        bool isExpired() override
        {
            return 0 == getInt();
        }

        void increment(Ttl ttl, uint8_t step) override
        {
            count += step;

            if (!ttl.isForever()) queue.add(ttl, step);
        }

        const std::string get() override
        {
            return std::to_string(getInt());
        }

        private:
        uint16_t getInt()
        {
            return count -= queue.clearExpired();
        }
    };

    CounterEntry* initCounterEntry()
    {
        return new CCounterEntry();
    }
}