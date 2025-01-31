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
            std::runtime_error("Entry '" + id.toString() + "' is of type '" + typeToString(type) + "'")
        {}

        private:
        static std::string typeToString(Entry::Type type)
        {
            switch (type) {
                case Entry::Type::KEEPER: return "KEEPER";
                case Entry::Type::COUNTER: return "COUNTER";
                default: return "UNKNOWN";
            }
        }
    };

    class MemoryStorage : public Storage
    {
        private:
        std::unordered_map<std::string, Entry::Entry*> entries;

        public:
        void clear(const Entry::Id id) override
        {
            if (!entries.contains(id.toString())) {
                return;
            }

            delete entries[id.toString()];
            entries.erase(id.toString());
        }

        void truncate(const std::string scope) override
        {
            std::string prefix = scope + ":";
            std::vector<std::string> to_erase;

            for (const auto& [id, entry] : entries) {
                if (id.find(prefix) == 0) {
                    to_erase.push_back(id);
                }
            }

            for (const auto& id : to_erase) {
                entries.erase(id);
                // delete entries[id]; TODO fix me
            }
        }

        Entry::Entry* find(const Entry::Id id) override
        {
            if (!entries.contains(id.toString())) {
                return nullptr;
            }

            Entry::Entry* entry = entries[id.toString()];

            if (!entry->isExpired()) {
                return entry;
            }

            entries.erase(id.toString());
            return nullptr;
        }

        Entry::CounterEntry* getCounter(const Entry::Id id) override
        {
            return static_cast<Entry::CounterEntry*>(get(id, Entry::Type::COUNTER));
        }

        Entry::KeeperEntry* getKeeper(const Entry::Id id) override
        {
            return static_cast<Entry::KeeperEntry*>(get(id, Entry::Type::KEEPER));
        }

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
            Entry::Entry* entry = find(id) ?: entries[id.toString()] = create(id, type);

            if (entry->type() != type) {
                throw EntryTypeMismatchException(id, entry->type());
            }

            return entry;
        }

        Entry::Entry* create(const Entry::Id id, Entry::Type type)
        {
            switch (type) {
                case Entry::Type::COUNTER: return Entry::initCounterEntry();
                case Entry::Type::KEEPER: return Entry::initKeeperEntry();
                case Entry::Type::QUEUE: return Entry::initQueueEntry();
                case Entry::Type::RATER: return Entry::initRaterEntry();
                default: throw std::runtime_error("Unsupported entry type '" + id.toString() + "'");
            }
        }
    };

    Storage* empty()
    {
        return new MemoryStorage();
    }
}