#include <unordered_map>
#include <cstdint>

#include "../Command/Command.h"
#include "../Storage/Entry.h"
#include "../Storage/Storage.h"

namespace Sider::Storage::Entry
{
    struct RExpiringCounts
    {
        private:
        const Ttl ttl;
        const uint8_t partition;
        const uint32_t countStep;
        RExpiringCounts* next;
        RExpiringCounts* prev;

        public:
        RExpiringCounts() :
            ttl(Ttl::forever()),
            partition(0),
            countStep(0)
        {
            next = prev = this;
        }

        void add(Ttl ttl, uint8_t partition, uint32_t countStep)
        {
            RExpiringCounts* newCount = new RExpiringCounts(ttl, partition, countStep);
            RExpiringCounts* current = next;

            while (newCount->expiresBeforeThan(current)) {
                current = current->next;
            }

            newCount->next = current;
            newCount->prev = current->prev;
            current->prev->next = newCount;
            current->prev = newCount;
        }

        std::unordered_map<uint8_t, uint32_t> clearExpired()
        {
            std::unordered_map<uint8_t, uint32_t> expiredCounts;
            RExpiringCounts* current = prev;

            while (current->ttl.isExpired()) {
                expiredCounts[current->partition] += current->countStep;
                current = current->prev;

                delete current->next;
            }

            current->next = this;
            prev = current;

            return expiredCounts;
        }

        private:
        RExpiringCounts(Ttl ttl, uint8_t partition, uint32_t countStep) :
            ttl(ttl),
            partition(partition),
            countStep(countStep),
            next(nullptr),
            prev(nullptr)
        {}

        bool expiresBeforeThan(const RExpiringCounts* that) const
        {
            return ttl.expiresBeforeThan(that->ttl);
        }
    };

    class RaterEntry : public Entry
    {
        private:
        uint16_t totalCount;
        RExpiringCounts expiringCounts;
        std::unordered_map<uint8_t, uint16_t> counts;
        
        public:
        RaterEntry() :
            totalCount(0),
            expiringCounts(RExpiringCounts()),
            counts()
        {}

        Type type() const override
        {
            return Type::RATER;
        }

        bool isExpired() override
        {
            clearExpired();

            return totalCount == 0;
        }

        void increment(Ttl ttl, uint8_t partition, uint16_t step)
        {
            totalCount += step;
            counts[partition] += step;

            if (!ttl.isForever()) expiringCounts.add(ttl, partition, step);
        }

        const std::string get() override
        {
            return get(0);
        }

        const std::string get(uint8_t partition) override
        {
            clearExpired();

            return std::to_string(static_cast<uint16_t>((100.0f * counts[partition]) / totalCount));
        }

        private:
        void clearExpired()
        {
            std::unordered_map<uint8_t, uint32_t> expiredCounts = expiringCounts.clearExpired();

            for (const auto& [partition, count] : expiredCounts) {
                counts[partition] -= count;
                totalCount -= count;
            }
        }
    };
}

namespace Sider::Command
{
    using namespace Sider::Storage;

    class RateCommand : public Command
    {
        private:
        const Entry::Id id;
        const Entry::Ttl ttl;
        const uint8_t partition;
        const uint16_t step;

        public:
        RateCommand(Entry::Id id, Entry::Ttl ttl, uint8_t partition, uint16_t step) :
            id(id),
            ttl(ttl),
            partition(partition),
            step(step)
        {}

        Result execute(Sider::Storage::Storage* storage) override
        {
            // todo add test when returned type is not RaterEntry

            Entry::Entry* entry = storage->find(id);

            if (entry == nullptr) {
                Entry::RaterEntry* rater = new Entry::RaterEntry();
                rater->increment(ttl, partition, step);
                storage->add(id, rater);
            } else {
                Entry::RaterEntry* rater = static_cast<Entry::RaterEntry*>(entry);
                rater->increment(ttl, partition, step);
            }

            return Result::ok();
        }
    };

    Command* rate(std::string scope, std::string key, uint8_t partition, uint16_t step)
    {
        return new RateCommand(Entry::Id{scope, key}, Entry::Ttl::forever(), partition, step);
    }

    Command* rate(std::string scope, std::string key, uint8_t partition, uint16_t step, uint32_t ttl)
    {
        return new RateCommand(Entry::Id{scope, key}, Entry::Ttl::expiringIn(ttl), partition, step);
    }
}
