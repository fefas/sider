#include <memory>
#include <string>

#include "Command.h"
#include "../Storage/Entry.h"
#include "../Storage/Storage.h"

namespace Sider::Command
{
    using namespace Sider::Storage;

    class KeepCommand : public Command
    {
        private:
        const Entry::Id id;
        const Entry::Ttl ttl;
        const std::string value;

        public:
        KeepCommand(Entry::Id id, Entry::Ttl ttl, std::string value) :
            id(id),
            ttl(ttl),
            value(value)
        {}

        Result execute(Sider::Storage::Storage* storage) override
        {
            // try { TODO add test
                storage
                    ->getKeeper(id)
                    ->keep(ttl, value);
            // } catch (const std::runtime_error& e) {
            //     return Result::error(e.what());
            // }

            return Result::ok();
        }
    };

    Command* keep(std::string scope, std::string key, std::string value)
    {
        return new KeepCommand(Entry::Id{scope, key}, Entry::Ttl::forever(), value);
    }

    Command* keep(std::string scope, std::string key, std::string value, uint32_t ttl)
    {
        return new KeepCommand(Entry::Id{scope, key}, Entry::Ttl::expiringIn(ttl), value);
    }
}