#ifndef SIDER_COMMAND_H
#define SIDER_COMMAND_H

#include <cstdint>
#include <string>

#include "../Registry/Registry.h"

namespace Sider::Command
{
    using namespace Sider;
    using namespace std;

    struct Result
    {
        const bool success;
        const string value;

        static Result ok() { return Result{true, "ok"}; }
        static Result nil() { return Result{false, "nil"}; }
        static Result with(uint32_t value) { return Result{true, to_string(value)}; }
        static Result with(string value) { return Result{true, value}; }
        static Result error(string message) { return Result{false, message}; }
    };

    struct Command
    {
        virtual Result execute(Registry::Registry* registry) = 0;
    };

    Command* get(string scope, string key);
    Command* keep(string scope, string key, string value, uint32_t ttl = 0);
    Command* count(string scope, string key, uint32_t ttl = 0);
    // Command* rate(string scope, string key);
}

#endif
