<?php

namespace Sider;

interface Client
{
    public function set(string $key, string $value, ?string $ttl = null): void;
    public function count(string $key): void;
    public function rate(string $key, Rate $rate): void;
    public function get(string $key): ?string;
    public function disconnect(): void;
}
