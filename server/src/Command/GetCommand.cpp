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
                case Entry::Type::COUNTER:
                case Entry::Type::KEEPER:
                case Entry::Type::QUEUE:
                    return Result::with(entry->get());

                case Entry::Type::RATER:
                    return Result::with(entry->get(partition));

                default: {
                    throw std::runtime_error("Unsupported entry type '" + id.scope.name + ":" + id.key.name + "'");
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