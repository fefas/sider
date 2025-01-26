<?php

namespace Sider;

use PHPUnit\Framework\Attributes\DataProvider;
use PHPUnit\Framework\Attributes\Test;

final class KeepTest extends ClientTestCase
{

    public static function keysWithExpectedValues(): iterable
    {
        yield ['keep-key1', 'value1'];
        yield ['keep-key2', 'value2'];
        yield ['keep-key3', 'value3'];
        yield ['keep-key4', 'value4'];
    }

    #[Test, DataProvider('keysWithExpectedValues')]
    public function getKeptValueByItsKey(string $key, string $expected): void
    {
        $this->client->keep('keep-key1', 'value1');
        $this->client->keep('keep-key2', 'value2');
        $this->client->keep('keep-key3', 'value3');
        $this->client->keep('keep-key4', 'value4');

        $actual = $this->client->get($key);

        self::assertEquals($expected, $actual);
    }

    #[Test]
    public function getLatestKeptValueByItsKey(): void
    {
        $this->client->keep('keep-key', 'value1');
        $this->client->keep('keep-key', 'value2');

        $actual = $this->client->get('keep-key');

        self::assertEquals('value2', $actual);
    }

    #[Test]
    public function clear(): void
    {
        $this->client->keep('keep-key', 'value1');

        $this->client->clear('keep-key');

        self::assertNull($this->client->get('keep-key'));
    }

    #[Test]
    public function getNullIfNoValueWasStoredByProvidedKey(): void
    {
        $actual = $this->client->get('nkey');

        self::assertNull($actual);
    }
}
