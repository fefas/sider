#pragma once

#include <cstdint>
#include <string>

#include "../Storage/Storage.h"

namespace Sider::Command
{
    struct Result
    {
        const bool success;
        const std::string value;

        static Result ok() { return Result{true, "ok"}; }
        static Result nil() { return Result{false, "nil"}; }
        static Result with(uint32_t value) { return Result{true, std::to_string(value)}; }
        static Result with(std::string value) { return Result{true, value}; }
        static Result error(std::string message) { return Result{false, message}; }
    };

    struct Command
    {
        virtual Result execute(Sider::Storage::Storage* storage) = 0;
    };

    Command* get(std::string scope, std::string key);
    Command* get(std::string scope, std::string key, uint8_t partition);

    Command* clear(std::string scope, std::string key);

    Command* truncate(std::string scope);

    Command* keep(std::string scope, std::string key, std::string value);
    Command* keep(std::string scope, std::string key, std::string value, uint32_t ttl);

    Command* count(std::string scope, std::string key, uint8_t step);
    Command* count(std::string scope, std::string key, uint8_t step, uint32_t ttl);

    Command* rate(std::string scope, std::string key, uint8_t partition, uint16_t step);
    Command* rate(std::string scope, std::string key, uint8_t partition, uint16_t step, uint32_t ttl);

    Command* queue(std::string scope, std::string key, std::string value);
    Command* dequeue(std::string scope, std::string key);
}
