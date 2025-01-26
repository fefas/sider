#include <unordered_map>

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

        Entry::KeeperEntry* getKeeper(const Entry::Id id) override
        {
            return static_cast<Entry::KeeperEntry*>(get(id, Entry::Type::KEEPER));
        }

        Entry::CounterEntry* getCounter(const Entry::Id id) override
        {
            return static_cast<Entry::CounterEntry*>(get(id, Entry::Type::COUNTER));
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
                default: return Entry::initKeeperEntry();
            }
        }
    };

    Storage* empty()
    {
        return new MemoryStorage();
    }
}