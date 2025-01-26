<?php

namespace Sider;

use PHPUnit\Framework\Attributes\DataProvider;
use PHPUnit\Framework\Attributes\Test;

final class CountTest extends ClientTestCase
{
    public static function keysWithExpectedCount(): iterable
    {
        yield ['key1', 1];
        yield ['key2', 8];
    }

    #[Test, DataProvider('keysWithExpectedCount')]
    public function getCountByKey(string $key, int $expected): void
    {
        $this->client->count('key1', 1);
        $this->client->count('key2', 2);
        $this->client->count('key2', 2);
        $this->client->count('key2', 2);
        $this->client->count('key2', 2);

        $actual = $this->client->get($key);

        self::assertEquals($expected, $actual);
    }
}
