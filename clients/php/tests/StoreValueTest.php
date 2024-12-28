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

        $this->client->set('test', 'value');
        $this->client->set('test2', 'value1');
        $this->client->set('test3', 'value2');
        $this->client->set('test4', 'value3');
    }

    #[After]
    public function disconnectClient(): void
    {
        $this->client->disconnect();
    }

    public static function keysWithExpectedValues(): iterable
    {
        yield 'test' => ['test', 'value'];
        yield 'test2' => ['test2', 'value1'];
        yield 'test3' => ['test3', 'value2'];
        yield 'test4' => ['test4', 'value3'];
    }

    #[Test, DataProvider('keysWithExpectedValues')]
    public function canRetrieveAStoragedValueByItsKey(string $key, string $expectedValue): void
    {
        self::assertEquals($expectedValue, $this->client->get($key));
    }
}
