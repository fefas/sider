<?php

namespace Sider;

use PHPUnit\Framework\Attributes\After;
use PHPUnit\Framework\Attributes\Before;
use PHPUnit\Framework\Attributes\DataProvider;
use PHPUnit\Framework\Attributes\Test;
use PHPUnit\Framework\TestCase;
use Sider\Client;

final class RateTest extends TestCase
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
    public function getRateByKey(): void
    {
        $this->client->rate('key-rate', Rate::TOTAL);
        $this->client->rate('key-rate', Rate::TOTAL);
        $this->client->rate('key-rate', Rate::PART);
        $this->client->rate('key-rate', Rate::TOTAL);

        $actual = $this->client->get('key-rate');

        self::assertEquals('0.25', $actual);
    }
}
