#include <unordered_map>
#include <vector>

#include "Entry.h"
#include "Storage.h"

namespace Sider::Storage
{
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
    };

    Storage* empty()
    {
        return new MemoryStorage();
    }
}