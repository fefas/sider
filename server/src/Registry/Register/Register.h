#ifndef SIDER_REGISTRY_REGISTER_H
#define SIDER_REGISTRY_REGISTER_H

#include <string>
#include "Ttl.h"

namespace Sider::Registry::Register
{
    using namespace std;

    struct Id
    {
        const string scope;
        const string key;

        static Id* fromString(const string scope, const string key)
        {
            return new Id(scope, key);
        }

        string toString() const
        {
            return scope + "." + key;
        }
    };

    enum Type { COUNT, KEEPER };

    struct Register
    {
        const Id id;
        const Type type;

        protected:
        Register(const Id id, Type t) : id(id), type(t) {}

        public:
        virtual bool isExpired() = 0;
        virtual ~Register() = 0;
    };
    
    struct CounterRegister : public Register
    {
        virtual void increment(const Ttl* ttl) = 0;
        virtual const uint32_t get() = 0;

        protected:
        CounterRegister(const Id* id) : Register(*id, Type::COUNT) {}
        virtual ~CounterRegister() = 0;
    };
    
    struct KeeperRegister : public Register
    {
        virtual void keep(const Ttl* ttl, const string value) = 0;
        virtual const string get() = 0;

        protected:
        KeeperRegister(const Id* id) : Register(*id, Type::KEEPER) {}
        virtual ~KeeperRegister() = 0;
    };

    CounterRegister* createCounterRegister(const Id* id);
    KeeperRegister* createKeeperRegister(const Id* id);
}

#endif
