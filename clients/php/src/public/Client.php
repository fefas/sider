<?php

namespace Sider;

interface Client
{
    public function set(string $key, string $value): void;
    public function get(string $key): string;
    public function disconnect(): void;
}
