#include <string>

#include "Command.h"
#include "../Storage/Entry.h"
#include "../Storage/Storage.h"

namespace Sider::Command
{
    using namespace Sider::Storage;

    class CDequeueCommand : public Command
    {
        private:
        const Entry::Id id;

        public:
        CDequeueCommand(Entry::Id id) :
            id(id)
        {}

        Result execute(Sider::Storage::Storage* storage) override
        {
            // try { TODO add test
                storage
                    ->getQueue(id)
                    ->dequeue();
            // } catch (const std::runtime_error& e) {
            //     return Result::error(e.what());
            // }

            return Result::ok();
        }
    };

    Command* dequeue(std::string scope, std::string key)
    {
        return new CDequeueCommand(Entry::Id{scope, key});
    }
}