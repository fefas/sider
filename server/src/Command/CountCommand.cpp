#include <cstdint>
#include <string>

#include "Command.h"
#include "../Storage/Storage.h"

using namespace Sider;

namespace Sider::Command
{
    using namespace Sider::Storage;

    class CountCommand : public Command
    {
        private:
        const Entry::Id id;
        const Entry::Ttl ttl;
        const uint8_t step;

        public:
        CountCommand(Entry::Id id, Entry::Ttl ttl, uint8_t step) :
            id(id),
            ttl(ttl),
            step(step)
        {}

        Result execute(Sider::Storage::Storage* storage) override
        {
            try {
                storage
                    ->getCounter(id)
                    ->increment(ttl, step);
            } catch (const std::runtime_error &e) {
                return Result::error(e.what());
            }

            return Result::ok();
        }
    };

    Command* count(std::string scope, std::string key, uint8_t step)
    {
        return new CountCommand(Entry::Id{scope, key}, Entry::Ttl::forever(), step);
    }

    Command* count(std::string scope, std::string key, uint8_t step, uint32_t ttl)
    {
        return new CountCommand(Entry::Id{scope, key}, Entry::Ttl::expiringIn(ttl), step);
    }
}