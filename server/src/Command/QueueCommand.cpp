#include <string>

#include "Command.h"
#include "../Storage/Entry.h"
#include "../Storage/Storage.h"

namespace Sider::Command
{
    using namespace Sider::Storage;

    class QueueCommand : public Command
    {
        private:
        const Entry::Id id;
        const std::string value;

        public:
        QueueCommand(Entry::Id id, std::string value) :
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
        return new QueueCommand(Entry::Id{scope, key}, value);
    }
}