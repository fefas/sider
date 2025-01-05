#include <cstdint>
#include <stdexcept>
#include <string>
#include <iostream>

#include "../Register.h"
#include "../Ttl.h"

namespace Sider::Registry::Register
{
    using namespace std;

    class CKeeperRegister : public KeeperRegister
    {
        private:
        Expiration* expiration; // TODO: use smart pointer
        string value;

        public:
        CKeeperRegister(const Id* id) :
            KeeperRegister(id),
            expiration(nullptr)
        {}

        ~CKeeperRegister() override
        {
            if (expiration) {
                delete expiration;
            }
        }

        void keep(const Ttl* ttl, const string newValue) override
        {
            if (expiration) {
                delete expiration;
            }

            expiration = ttl ? Expiration::withTtl(ttl) : nullptr;
            value = newValue;
        }

        const string get() override
        {
            return value;
        }

        bool isExpired() override
        {
            return expiration && expiration->isPassed();
        }
    };

    KeeperRegister* createKeeperRegister(const Id* id)
    {
        return new CKeeperRegister(id);
    }
}