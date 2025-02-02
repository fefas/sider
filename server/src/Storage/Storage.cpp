#include <unordered_map>
#include <vector>

#include "Entry.h"
#include "Storage.h"

namespace Sider::Storage
{
    class EntryTypeMismatchException : public std::runtime_error
    {
        public:
        EntryTypeMismatchException(const Entry::Id id, Entry::Type type) :
            std::runtime_error("Entry '" + id.scope.name + ":" + id.key.name + "' is of type '" + typeToString(type) + "'")
        {}

        private:
        static std::string typeToString(Entry::Type type)
        {
            switch (type) {
                case Entry::Type::KEEPER: return "KEEPER";
                case Entry::Type::COUNTER: return "COUNTER";
                // TODO test more cases
                default: return "UNKNOWN";
            }
        }
    };

    class MemoryStorage : public Storage
    {
        private:
        std::unordered_map<std::string, std::unordered_map<std::string, Entry::Entry*>> entries;

        public:
        void clear(const Entry::Id id) override
        {
            if (!entries.contains(id.scope.name) || !entries[id.scope.name].contains(id.key.name)) {
                return;
            }

            delete entries[id.scope.name][id.key.name];
            entries[id.scope.name].erase(id.key.name);
        }

        void clear(const Entry::Scope scope) override
        {
            for (const auto & [_, entry] : entries[scope.name]) {
                delete entry;
            }

            entries[scope.name].clear();
        }

        void add(const Entry::Id id, Entry::Entry *entry) override
        {
            // todo do not allow to add entry with same id
            entries[id.scope.name][id.key.name] = entry;
        }   

        Entry::Entry* find(const Entry::Id id) override
        {
            if (!entries[id.scope.name].contains(id.key.name)) {
                return nullptr;
            }

            Entry::Entry* entry = entries[id.scope.name][id.key.name];

            if (!entry->isExpired()) {
                return entry;
            }

            entries[id.scope.name].erase(id.key.name);
            return nullptr;
        }

        Entry::CounterEntry* getCounter(const Entry::Id id) override
        {
            return static_cast<Entry::CounterEntry*>(get(id, Entry::Type::COUNTER));
        }

        // Entry::KeeperEntry* getKeeper(const Entry::Id id) override
        // {
        //     return static_cast<Entry::KeeperEntry*>(get(id, Entry::Type::KEEPER));
        // }

        Entry::QueueEntry* getQueue(const Entry::Id id) override
        {
            return static_cast<Entry::QueueEntry*>(get(id, Entry::Type::QUEUE));
        }

        Entry::RaterEntry* getRater(const Entry::Id id) override
        {
            return static_cast<Entry::RaterEntry*>(get(id, Entry::Type::RATER));
        }

        private:
        Entry::Entry* get(const Entry::Id id, Entry::Type type)
        {
            Entry::Entry* entry = find(id) ?: entries[id.scope.name][id.key.name] = create(id, type);

            if (entry->type() != type) {
                throw EntryTypeMismatchException(id, entry->type());
            }

            return entry;
        }

        Entry::Entry* create(const Entry::Id id, Entry::Type type)
        {
            switch (type) {
                case Entry::Type::COUNTER: return Entry::initCounterEntry();
                // case Entry::Type::KEEPER: return Entry::initKeeperEntry();
                case Entry::Type::QUEUE: return Entry::initQueueEntry();
                case Entry::Type::RATER: return Entry::initRaterEntry();
                default: throw std::runtime_error("Unsupported entry type '" + id.scope.name + ":" + id.key.name + "'");
            }
        }
    };

    Storage* empty()
    {
        return new MemoryStorage();
    }
}