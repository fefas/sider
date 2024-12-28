<?php

namespace Sider;

use PHPUnit\Framework\Attributes\After;
use PHPUnit\Framework\Attributes\Before;
use PHPUnit\Framework\Attributes\DataProvider;
use PHPUnit\Framework\Attributes\Test;
use PHPUnit\Framework\TestCase;
use Sider\Client;

final class CountTest extends TestCase
{
    private Client $client;

    #[Before]
    public function setUpClient(): void
    {
        $this->client = ClientSetting::withUrl($_ENV['SIDER_URL'])->build();
    }

    #[After]
    public function disconnectClient(): void
    {
        $this->client->disconnect();
    }

    #[Test]
    public function getCountByKey(): void
    {
        $this->client->count('key');
        $this->client->count('key');
        $this->client->count('key');
        $this->client->count('key');

        $actual = $this->client->get('key');

        self::assertEquals('4', $actual);
    }
}
