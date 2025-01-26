#include <string>

#include "Command.h"
#include "../Storage/Entry.h"
#include "../Storage/Storage.h"

namespace Sider::Command
{
    using namespace Sider::Storage;

    class CQueueCommand : public Command
    {
        private:
        const Entry::Id id;
        const std::string value;

        public:
        CQueueCommand(Entry::Id id, std::string value) :
            id(id),
            value(value)
        {}

        Result execute(Sider::Storage::Storage* storage) override
        {
            // try { TODO add test
                storage
                    ->getQueue(id)
                    ->queue(value);
            // } catch (const std::runtime_error& e) {
            //     return Result::error(e.what());
            // }

            return Result::ok();
        }
    };

    Command* queue(std::string scope, std::string key, std::string value)
    {
        return new CQueueCommand(Entry::Id{scope, key}, value);
    }
}