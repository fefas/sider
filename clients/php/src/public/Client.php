<?php

namespace Sider;

interface Client
{
    public function clear(string $key): void;
    public function get(string $key, ?int $partition = null): ?string;
    public function count(string $key, int $step = 1, ?int $ttl = null): void;
    public function keep(string $key, string $value, ?int $ttl = null): void;
    public function rate(string $key, int $partition, int $step = 1, ?int $ttl = null): void;
    public function queue(string $key, string $value): void;
    public function dequeue(string $key): void;
    public function disconnect(): void;
}
