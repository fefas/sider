#include <unordered_map>
#include <cstdint>
#include "../Entry.h"

#include <iostream>

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
            if (current->prev == nullptr) {
                std::cout << "add3" << std::endl;
            }

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

    class CRaterEntry : public RaterEntry
    {
        private:
        uint16_t totalCount;
        RExpiringCounts expiringCounts;
        std::unordered_map<uint8_t, uint16_t> counts;
        
        public:
        CRaterEntry() :
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

        void increment(Ttl ttl, uint8_t partition, uint16_t step) override
        {
            totalCount += step;
            counts[partition] += step;

            if (!ttl.isForever()) expiringCounts.add(ttl, partition, step);
        }

        const uint16_t get(uint8_t partition) override
        {
            clearExpired();

            return static_cast<uint16_t>((100.0f * counts[partition]) / totalCount);
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

    RaterEntry* initRaterEntry()
    {
        return new CRaterEntry();
    }
}