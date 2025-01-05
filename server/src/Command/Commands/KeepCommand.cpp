#include <iostream>
#include <string>

#include "../Command.h"
#include "../../Registry/Register/Register.h"
#include "../../Registry/Registry.h"

namespace Sider::Command
{
    using namespace Sider::Registry::Register;
    using namespace std;

    class KeepCommand : public Command
    {
        private:
        const string scope;
        const string key;
        const string value;
        const uint32_t ttl;

        public:
        KeepCommand(string scope, string key, uint32_t ttl, string value) :
            scope(scope),
            key(key),
            ttl(ttl),
            value(value)
        {}

        Result execute(Sider::Registry::Registry* registry) override
        {
            registry
                ->getKeeper(Id::fromString(scope, key))
                ->keep(ttl != 0 ? Ttl::inSeconds(ttl) : nullptr, value);

            return Result::ok();
        }
    };

    Command* keep(string scope, string key, string value, uint32_t ttl)
    {
        return new KeepCommand(scope, key, ttl, value);
    }
}