#include <stdexcept>
#include <string>
#include <unordered_map>
#include <iostream>

#include "Registry.h"
#include "Register/Register.h"

namespace Sider::Registry
{
    using namespace std;

    class CRegistry : public Registry
    {
        private:
        unordered_map<string, Register::Register*> registers;

        public:
        Register::Register* find(const Register::Id* id) override
        {
            if (!registers.contains(id->toString())) {
                return nullptr;
            }

            Register::Register* reg = registers[id->toString()];

            if (reg->isExpired()) {
                registers.erase(id->toString());
                delete reg;
                return nullptr;
            }

            return reg;
        }

        Register::CounterRegister* getCounter(const Register::Id* id) override
        {
            return (Register::CounterRegister*) get(id, Register::Type::COUNT);
        }

        Register::KeeperRegister* getKeeper(const Register::Id* id) override
        {
            return (Register::KeeperRegister*) get(id, Register::Type::KEEPER);
        }

        private:
        Register::Register* get(const Register::Id* id, Register::Type type)
        {
            Register::Register* reg = find(id) ?: create(id, type);

            if (reg->type != type) {
                throw runtime_error("Register [" + reg->id.toString() + "] mismatch");
            }

            return reg;
        }

        Register::Register* create(const Register::Id* id, Register::Type type)
        {
            Register::Register* newRegister;

            switch (type) { 
                case Register::Type::COUNT:
                    newRegister = Register::createCounterRegister(id);
                    break;
                case Register::Type::KEEPER:
                    newRegister = Register::createKeeperRegister(id);
                    break;
                default:
                    throw runtime_error("Unsupported register type: " + to_string(type));
            }

            return registers[id->toString()] = newRegister;
        }
    };

    Registry* createEmpty()
    {
        return new CRegistry();
    }
}
