#pragma once

#include <string>

#include "Entry.h"

namespace Sider::Storage
{
    class Storage
    {
        public:
        virtual void clear(const Entry::Id id) = 0;
        virtual Entry::Entry* find(const Entry::Id id) = 0;
        virtual Entry::CounterEntry* getCounter(const Entry::Id id) = 0;
        virtual Entry::KeeperEntry* getKeeper(const Entry::Id id) = 0;
        virtual Entry::QueueEntry* getQueue(const Entry::Id id) = 0;
    };

    Storage* empty();
}
