#include <string>

#include "../Command/Command.h"
#include "../Storage/Entry.h"
#include "../Storage/Storage.h"

namespace Sider::Storage::Entry
{
    struct KeeperEntry : public Entry, public NonPartitionedEntry
    {
        private:
        Ttl ttl;
        std::string value;

        public:
        KeeperEntry(Ttl ttl, std::string value) :
            ttl(ttl),
            value(value)
        {}

        Type type() const override
        {
            return Type::KEEPER;
        }

        bool isExpired() override
        {
            return ttl.isExpired();
        }

        const std::string get() override
        {
            return value;
        }

        void keep(Ttl newTtl, std::string newValue)
        {
            ttl = newTtl;
            value = newValue;
        }
    };
}

namespace Sider::Command
{
    using namespace Sider::Storage;

    class KeepCommand : public Command
    {
        private:
        const Entry::Id id;
        const Entry::Ttl ttl;
        const std::string value;

        public:
        KeepCommand(Entry::Id id, Entry::Ttl ttl, std::string value) :
            id(id),
            ttl(ttl),
            value(value)
        {}

        Result execute(Sider::Storage::Storage* storage) override
        {
            // todo add test when returned type is not KeeperEntry

            Entry::Entry* entry = storage->find(id);

            if (entry == nullptr) {
                storage->add(id, new Entry::KeeperEntry(ttl, value));
            } else {
                Entry::KeeperEntry* keeper = static_cast<Entry::KeeperEntry*>(entry);
                keeper->keep(ttl, value);
            }

            return Result::ok();
        }
    };

    Command* keep(std::string scope, std::string key, std::string value)
    {
        return new KeepCommand(Entry::Id{scope, key}, Entry::Ttl::forever(), value);
    }

    Command* keep(std::string scope, std::string key, std::string value, uint32_t ttl)
    {
        return new KeepCommand(Entry::Id{scope, key}, Entry::Ttl::expiringIn(ttl), value);
    }
}