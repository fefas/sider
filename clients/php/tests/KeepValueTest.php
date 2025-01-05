<?php

namespace Sider;

use PHPUnit\Framework\Attributes\DataProvider;
use PHPUnit\Framework\Attributes\Test;

final class KeepValueTest extends ClientTestCase
{

    public static function keysWithExpectedValues(): iterable
    {
        yield 'test1' => ['keep-key1', 'value1'];
        yield 'test2' => ['keep-key2', 'value2'];
        yield 'test3' => ['keep-key3', 'value3'];
        yield 'test4' => ['keep-key4', 'value4'];
    }

    #[Test, DataProvider('keysWithExpectedValues')]
    public function getStoragedValueByItsKey(string $key, string $expected): void
    {
        $this->client->keep('keep-key1', 'value1');
        $this->client->keep('keep-key2', 'value2');
        $this->client->keep('keep-key3', 'value3');
        $this->client->keep('keep-key4', 'value4');

        $actual = $this->client->get($key);

        self::assertEquals($expected, $actual);
    }

    #[Test]
    public function getLatestStoragedValueByItsKey(): void
    {
        $this->client->keep('keep-key', 'value1');
        $this->client->keep('keep-key', 'value2');

        $actual = $this->client->get('keep-key');

        self::assertEquals('value2', $actual);
    }


    #[Test]
    public function getNullIfNoValueWasStoredByProvidedKey(): void
    {
        $actual = $this->client->get('nkey');

        self::assertNull($actual);
    }
}
