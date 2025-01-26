<?php

namespace Sider;

use PHPUnit\Framework\Attributes\After;
use PHPUnit\Framework\Attributes\Before;
use PHPUnit\Framework\TestCase;
use Ramsey\Uuid\Uuid;

abstract class ClientTestCase extends TestCase
{
    protected Client $client;

    #[Before]
    public function setUpClient(): void
    {
        $this->client = ClientSetting::withUrl($_ENV['SIDER_URL'])
            ->withScope(Uuid::uuid4()->toString())
            ->build();
    }

    #[After]
    public function disconnectClient(): void
    {
        $this->client->disconnect();
    }
}
