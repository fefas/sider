<?php

namespace Sider;

use PHPUnit\Framework\Attributes\After;
use PHPUnit\Framework\Attributes\Before;
use PHPUnit\Framework\Attributes\DataProvider;
use PHPUnit\Framework\Attributes\Test;
use PHPUnit\Framework\TestCase;
use Sider\Client;

final class StoreValueTest extends TestCase
{
    private Client $client;

    #[Before]
    public function setUpClient(): void
    {
        $this->client = ClientSetting::withUrl($_ENV['SIDER_URL'])->build();

        $this->client->set('key1', 'value1');
        $this->client->set('key2', 'value2');
        $this->client->set('key3', 'value3');
        $this->client->set('key4', 'value4');
    }

    #[After]
    public function disconnectClient(): void
    {
        $this->client->disconnect();
    }

    public static function keysWithExpectedValues(): iterable
    {
        yield 'test' => ['key1', 'value1'];
        yield 'test2' => ['key2', 'value2'];
        yield 'test3' => ['key3', 'value3'];
        yield 'test4' => ['key4', 'value4'];
    }

    #[Test, DataProvider('keysWithExpectedValues')]
    public function getStoragedValueByItsKey(string $key, string $expected): void
    {
        $actual = $this->client->get($key);

        self::assertEquals($expected, $actual);
    }

    #[Test]
    public function getNullIfNoValueWasStoredByProvidedKey(): void
    {
        $actual = $this->client->get('nkey');

        self::assertNull($actual);
    }
}
