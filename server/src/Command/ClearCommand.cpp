#include <iostream>
#include <string>

#include "Command.h"
#include "../Storage/Storage.h"

namespace Sider::Command
{
    using namespace Sider::Storage;

    class ClearEntryCommand : public Command
    {
        private:
        const Entry::Id id;

        public:
        ClearEntryCommand(Entry::Id id) :
            id(id)
        {}

        Result execute(Sider::Storage::Storage* storage) override
        {
            storage->clear(id);

            return Result::ok();
        }
    };

    class ClearScopeCommand : public Command
    {
        private:
        const Entry::Scope scope;

        public:
        ClearScopeCommand(Entry::Scope scope) :
            scope(scope)
        {}

        Result execute(Sider::Storage::Storage* storage) override
        {
            storage->clear(scope);

            return Result::ok();
        }
    };

    Command* clear(std::string scope, std::string key)
    {
        return new ClearEntryCommand(Entry::Id{scope, key});
    }

    Command* clear(std::string scope)
    {
        return new ClearScopeCommand(Entry::Scope{scope});
    }
} 