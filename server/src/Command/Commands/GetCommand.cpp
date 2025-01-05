#include <iostream>
#include <string>

#include "../Command.h"
#include "../../Registry/Register/Register.h"
#include "../../Registry/Registry.h"

namespace Sider::Command
{
    using namespace Sider::Registry;
    using namespace std;

    class GetCommand : public Command
    {
        private:
        const string scope;
        const string key;

        public:
        GetCommand(string scope, string key) :
            scope(scope),
            key(key)
        {}

        Result execute(Sider::Registry::Registry* registry) override
        {
            Register::Register* reg = registry->find(Register::Id::fromString(scope, key));

            if (reg == nullptr) {
                return Result::nil();
            }

            switch (reg->type) {
                case Register::Type::COUNT: {
                    Register::CounterRegister* counter = (Register::CounterRegister*) reg;
                    return Result::with(counter->get());
                }

                case Register::Type::KEEPER: {
                    Register::KeeperRegister* keeper = (Register::KeeperRegister*) reg;
                    return Result::with(keeper->get());
                }
            }

            return Result::error("Unsupported register type: " + to_string(reg->type));
        }
    };

    Command* get(string scope, string key)
    {
        return new GetCommand(scope, key);
    }
}