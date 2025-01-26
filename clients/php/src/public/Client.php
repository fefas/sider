<?php

namespace Sider;

interface Client
{
    public function get(string $key): ?string;
    public function keep(string $key, string $value, ?int $ttl = null): void;
    public function count(string $key, int $step = 1, ?int $ttl = null): void;
    public function rate(string $key, Rate $rate, ?int $ttl = null): void;
    public function disconnect(): void;
}
