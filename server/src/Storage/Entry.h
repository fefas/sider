#pragma once

#include <memory>
#include <string>
#include "../Time.h"

namespace Sider::Storage::Entry
{
    enum Type { COUNTER, KEEPER, QUEUE, RATER };

    struct Scope { const std::string name; };
    struct Key { const std::string name; };
    struct Id { const Scope scope; const Key key; };

    struct Ttl
    {
        private:
        uint32_t expireAt;

        public:
        bool isForever() const { return expireAt == 0; }
        bool isExpired() const { return !isForever() && expireAt < Sider::Time::now(); }
        bool expiresBeforeThan(const Ttl that) const { return expireAt < that.expireAt; }

        static Ttl forever() { return Ttl{0}; }
        static Ttl expiringIn(uint32_t seconds) { return Ttl{Sider::Time::now() + seconds}; }

        private:
        Ttl(uint32_t expireAt) : expireAt(expireAt) {}
    };

    struct Entry
    {
        virtual Type type() const = 0;
        virtual bool isExpired() = 0;
        virtual ~Entry() = default;
    };
    
    struct CounterEntry : public Entry
    {
        virtual void increment(Ttl ttl, uint8_t step) = 0;
        virtual const uint16_t get() = 0;
    };
    
    struct KeeperEntry : public Entry
    {
        virtual void keep(Ttl ttl, std::string value) = 0;
        virtual const std::string get() = 0;
    };

    struct QueueEntry : public Entry
    {
        virtual void queue(std::string value) = 0;
        virtual void dequeue() = 0;
        virtual const std::string get() = 0;
    };

    struct RaterEntry : public Entry
    {
        virtual void increment(Ttl ttl, uint8_t partition, uint16_t step) = 0;
        virtual const uint16_t get(uint8_t partition) = 0;
    };

    KeeperEntry* initKeeperEntry();
    CounterEntry* initCounterEntry();
    QueueEntry* initQueueEntry();
    RaterEntry* initRaterEntry();
}
