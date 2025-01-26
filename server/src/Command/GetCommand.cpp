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
                default: {
                    throw std::runtime_error("Unsupported entry type '" + id.toString() + "'");
                }
            }
        }
    };

    Command* get(std::string scope, std::string key)
    {
        return new GetCommand(Entry::Id{scope, key});
    }
}