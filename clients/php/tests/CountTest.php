<?php

namespace Sider;

use PHPUnit\Framework\Attributes\Test;

final class CountTest extends ClientTestCase
{
    #[Test]
    public function getCountByKey(): void
    {
        $this->client->count('key2', 2);
        $this->client->count('key2', 2);
        $this->client->count('key2', 2);
        $this->client->count('key2', 2);

        $actual = $this->client->get('key2');

        self::assertEquals('4', $actual);
    }

    // #[Test]
    // public function ttl(): void
    // {
    //     $this->client->count('key3', 5 * 60);
    //     $this->client->count('key3', 5 * 60);

    //     $actual = $this->client->get('key3');

    //     self::assertEquals('2', $actual);
    // }
}
