#ifndef SIDER_REGISTRY_H
#define SIDER_REGISTRY_H

#include "Register/Register.h"

namespace Sider::Registry
{
    struct Registry
    {
        virtual Register::Register* find(const Register::Id* id) = 0;
        virtual Register::CounterRegister* getCounter(const Register::Id* id) = 0;
        virtual Register::KeeperRegister* getKeeper(const Register::Id* id) = 0;
    };

    Registry* createEmpty();
}

#endif
