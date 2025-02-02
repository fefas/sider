#pragma once

#include <string>

#include "Entry.h"

namespace Sider::Storage
{
    class Storage
    {
        public:
        virtual void clear(const Entry::Id id) = 0;
        virtual void clear(const Entry::Scope scope) = 0;

        virtual void add(const Entry::Id id, Entry::Entry *entry) = 0;

        virtual Entry::Entry* find(const Entry::Id id) = 0;
        virtual Entry::CounterEntry* getCounter(const Entry::Id id) = 0;
        virtual Entry::RaterEntry* getRater(const Entry::Id id) = 0;
    };

    Storage* empty();
}
