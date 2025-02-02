#include <iostream>
#include <string>

#include "Command.h"
#include "../Storage/Storage.h"

namespace Sider::Command
{
    using namespace Sider::Storage;

    class GetCommand : public Command
    {
        private:
        const Entry::Id id;

        public:
        GetCommand(Entry::Id id) :
            id(id)
        {}

        Result execute(Sider::Storage::Storage* storage) override
        {
            Entry::Entry* entry = storage->find(id);

            if (!entry) {
                return Result::nil();
            }

            // TODO add test is case it cannot cast
            Entry::NonPartitionedEntry* nonPartitionedEntry = dynamic_cast<Entry::NonPartitionedEntry*>(entry);

            return Result::with(nonPartitionedEntry->get());
        }
    };

    class GetPartitionCommand : public Command
    {
        private:
        const Entry::Id id;
        const uint8_t partition;

        public:
        GetPartitionCommand(Entry::Id id, uint8_t partition) :
            id(id),
            partition(partition)
        {}

        Result execute(Sider::Storage::Storage* storage) override
        {
            Entry::Entry* entry = storage->find(id);

            if (!entry) {
                return Result::nil();
            }

            // TODO add test is case it cannot cast
            Entry::PartitionedEntry* partitionedEntry = dynamic_cast<Entry::PartitionedEntry*>(entry);

            return Result::with(partitionedEntry->get(partition));
        }
    };

    Command* get(std::string scope, std::string key)
    {
        return new GetCommand(Entry::Id{scope, key});
    }

    Command* get(std::string scope, std::string key, uint8_t partition)
    {
        return new GetPartitionCommand(Entry::Id{scope, key}, partition);
    }   
}