#include <cstdint>
#include <string>

#include "Command.h"
#include "../Storage/Storage.h"

using namespace Sider;

namespace Sider::Command
{
    using namespace Sider::Storage;

    class RateCommand : public Command
    {
        private:
        const Entry::Id id;
        const Entry::Ttl ttl;
        const uint8_t partition;
        const uint16_t step;

        public:
        RateCommand(Entry::Id id, Entry::Ttl ttl, uint8_t partition, uint16_t step) :
            id(id),
            ttl(ttl),
            partition(partition),
            step(step)
        {}

        Result execute(Sider::Storage::Storage* storage) override
        {
            // try { TODO add test
                storage
                    ->getRater(id)
                    ->increment(ttl, partition, step);
            // } catch (const std::runtime_error &e) {
            //     return Result::error(e.what());
            // }

            return Result::ok();
        }
    };

    Command* rate(std::string scope, std::string key, uint8_t partition, uint16_t step)
    {
        return new RateCommand(Entry::Id{scope, key}, Entry::Ttl::forever(), partition, step);
    }

    Command* rate(std::string scope, std::string key, uint8_t partition, uint16_t step, uint32_t ttl)
    {
        return new RateCommand(Entry::Id{scope, key}, Entry::Ttl::expiringIn(ttl), partition, step);
    }
}