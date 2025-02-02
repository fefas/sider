#include <iostream>
#include <cstdint>
#include <stdexcept>

#include "../Command/Command.h"
#include "../Storage/Entry.h"
#include "../Storage/Storage.h"

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

    class CounterEntry : public Entry
    {
        private:
        ExpiringCounts queue;
        uint32_t count;

        public:
        CounterEntry() :
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

        void increment(Ttl ttl, uint8_t step)
        {
            count += step;

            if (!ttl.isForever()) queue.add(ttl, step);
        }

        const std::string get() override
        {
            return std::to_string(getInt());
        }

        const std::string get(uint8_t partition) override
        {
            return get();
        }

        private:
        uint16_t getInt()
        {
            return count -= queue.clearExpired();
        }
    };
}

namespace Sider::Command
{
    using namespace Sider::Storage;

    class CountCommand : public Command
    {
        private:
        const Entry::Id id;
        const Entry::Ttl ttl;
        const uint8_t step;

        public:
        CountCommand(Entry::Id id, Entry::Ttl ttl, uint8_t step) :
            id(id),
            ttl(ttl),
            step(step)
        {}

        Result execute(Sider::Storage::Storage* storage) override
        {
            // todo add test when returned type is not CounterEntry

            Entry::Entry* entry = storage->find(id);

            if (entry == nullptr) {
                Entry::CounterEntry* counter = new Entry::CounterEntry();
                counter->increment(ttl, step);
                storage->add(id, counter);
            } else {
                Entry::CounterEntry* counter = static_cast<Entry::CounterEntry*>(entry);
                counter->increment(ttl, step);
            }

            return Result::ok();
        }
    };

    Command* count(std::string scope, std::string key, uint8_t step)
    {
        return new CountCommand(Entry::Id{scope, key}, Entry::Ttl::forever(), step);
    }

    Command* count(std::string scope, std::string key, uint8_t step, uint32_t ttl)
    {
        return new CountCommand(Entry::Id{scope, key}, Entry::Ttl::expiringIn(ttl), step);
    }
}
