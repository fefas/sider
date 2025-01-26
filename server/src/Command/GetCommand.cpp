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

            if (entry->type() == Entry::Type::COUNTER) {
                Entry::CounterEntry* counterEntry = dynamic_cast<Entry::CounterEntry*>(entry);

                return Result::with(counterEntry->get());
            }

            Entry::KeeperEntry* keeperEntry = dynamic_cast<Entry::KeeperEntry*>(entry);

            return Result::with(keeperEntry->get());
        }
    };

    Command* get(std::string scope, std::string key)
    {
        return new GetCommand(Entry::Id{scope, key});
    }
}