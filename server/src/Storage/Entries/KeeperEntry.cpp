#include <string>

#include "../Entry.h"

namespace Sider::Storage::Entry
{
    class CKeeperEntry : public KeeperEntry
    {
        private:
        Ttl ttl;
        std::string value;

        public:
        CKeeperEntry(Ttl ttl, std::string value) :
            ttl(ttl),
            value(value)
        {}

        Type type() const override
        {
            return Type::KEEPER;
        }

        bool isExpired() override
        {
            return ttl.isExpired();
        }

        void keep(Ttl newTtl, std::string newValue) override
        {
            ttl = newTtl;
            value = newValue;
        }

        const std::string get() override
        {
            return value;
        }
    };

    KeeperEntry* initKeeperEntry()
    {
        return new CKeeperEntry(Ttl::forever(), "");
    }
}