#include <cstdint>
#include <string>

#include "../Command.h"
#include "../../Registry/Registry.h"
#include "../../Registry/Register/Ttl.h"

using namespace Sider;

namespace Sider::Command
{
    using namespace Sider::Registry::Register;
    using namespace std;

    class CountCommand : public Command
    {
        private:
        const string scope;
        const string key;
        const uint32_t ttl;

        public:
        CountCommand(string scope, string key, uint32_t ttl) :
            scope(scope),
            key(key),
            ttl(ttl)
        {}

        Result execute(Sider::Registry::Registry* registry) override
        {
            registry
                ->getCounter(Id::fromString(scope, key))
                ->increment(ttl != 0 ? Ttl::inSeconds(ttl) : nullptr);

            return Result::ok();
        }
    };

    Command* count(string scope, string key, uint32_t ttl)
    {
        return new CountCommand(scope, key, ttl);
    }
}