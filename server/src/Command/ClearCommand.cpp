#include <iostream>
#include <string>

#include "Command.h"
#include "../Storage/Storage.h"

namespace Sider::Command
{
    using namespace Sider::Storage;

    class ClearCommand : public Command
    {
        private:
        const Entry::Id id;

        public:
        ClearCommand(Entry::Id id) :
            id(id)
        {}

        Result execute(Sider::Storage::Storage* storage) override
        {
            storage->clear(id);

            return Result::ok();
        }
    };

    Command* clear(std::string scope, std::string key)
    {
        return new ClearCommand(Entry::Id{scope, key});
    }
} 