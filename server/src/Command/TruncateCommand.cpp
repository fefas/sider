#include <string>

#include "Command.h"
#include "../Storage/Storage.h"

namespace Sider::Command
{
    using namespace Sider::Storage;

    class TruncateCommand : public Command
    {
        private:
        const std::string scope;

        public:
        TruncateCommand(std::string scope) :
            scope(scope)
        {}

        Result execute(Sider::Storage::Storage* storage) override
        {
            storage->truncate(scope);

            return Result::ok();
        }
    };

    Command* truncate(std::string scope)
    {
        return new TruncateCommand(scope);
    }
} 