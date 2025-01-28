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
        const uint8_t partition;

        public:
        GetCommand(Entry::Id id, uint8_t partition) :
            id(id),
            partition(partition)
        {}

        Result execute(Sider::Storage::Storage* storage) override
        {
            Entry::Entry* entry = storage->find(id);

            if (!entry) {
                return Result::nil();
            }

            switch (entry->type()) {
                case Entry::Type::COUNTER: {
                    Entry::CounterEntry* counterEntry = dynamic_cast<Entry::CounterEntry*>(entry);

                    return Result::with(counterEntry->get());
                }
                case Entry::Type::KEEPER: {
                    Entry::KeeperEntry* keeperEntry = dynamic_cast<Entry::KeeperEntry*>(entry);

                    return Result::with(keeperEntry->get());
                }
                case Entry::Type::QUEUE: {
                    Entry::QueueEntry* queueEntry = dynamic_cast<Entry::QueueEntry*>(entry);

                    return Result::with(queueEntry->get());
                }
                case Entry::Type::RATER: {
                    Entry::RaterEntry* raterEntry = dynamic_cast<Entry::RaterEntry*>(entry);

                    return Result::with(raterEntry->get(partition));
                }
                default: {
                    throw std::runtime_error("Unsupported entry type '" + id.toString() + "'");
                }
            }
        }
    };

    Command* get(std::string scope, std::string key)
    {
        return new GetCommand(Entry::Id{scope, key}, 0);
    }

    Command* get(std::string scope, std::string key, uint8_t partition)
    {
        return new GetCommand(Entry::Id{scope, key}, partition);
    }   
}