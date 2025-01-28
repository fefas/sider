<?php

namespace Sider;

use PHPUnit\Framework\Attributes\DataProvider;
use PHPUnit\Framework\Attributes\Test;

final class RateTest extends ClientTestCase
{
    public static function keysWithExpectedRates(): iterable
    {
        yield ['key1', 0, 55];
        yield ['key2', 1, 45];
    }

    #[Test, DataProvider('keysWithExpectedRates')]
    public function getRateByKey(string $key, int $partition, int $expected): void
    {
        $this->client->rate($key, 0, 80);
        $this->client->rate($key, 1, 120);
        $this->client->rate($key, 0, 140);
        $this->client->rate($key, 1, 60);
        
        $actual = $this->client->get($key, $partition);

        self::assertEquals($expected, $actual);
    }

    #[Test]
    public function clear(): void
    {
        $this->client->rate('key', 0, 60);
        $this->client->rate('key', 1, 60);
        $this->client->clear('key');

        self::assertNull($this->client->get('key', 0));
        self::assertNull($this->client->get('key', 1));
    }
} 