<?php

namespace Sider;

use PHPUnit\Framework\Attributes\DataProvider;
use PHPUnit\Framework\Attributes\Test;

final class QueueTest extends ClientTestCase
{
    #[Test]
    public function getFirstQueuedValue(): void
    {
        $this->client->queue('queue1', 'v1');
        $this->client->queue('queue1', 'v2');
        $this->client->queue('queue1', 'v3');

        $actual = $this->client->get('queue1');

        self::assertEquals('v1', $actual);
    }

    #[Test]
    public function getNextQueuedValueAfterDequeue(): void
    {
        $this->client->queue('queue1', 'v1');
        $this->client->queue('queue1', 'v2');
        $this->client->queue('queue1', 'v3');
        $this->client->dequeue('queue1');

        $actual = $this->client->get('queue1');

        self::assertEquals('v2', $actual);
    }
    
    #[Test]
    public function getNilAfterAllQueuedValuesAreDequeued(): void
    {
        $this->client->queue('queue1', 'v1');
        $this->client->queue('queue1', 'v2');
        $this->client->queue('queue1', 'v3');
        $this->client->dequeue('queue1');
        $this->client->dequeue('queue1');
        $this->client->dequeue('queue1');

        $actual = $this->client->get('queue1');

        self::assertNull($actual);
    }

    public static function keysWithExpectedValues(): iterable
    {
        yield ['queue1', 'v1'];
        yield ['queue2', 'v1'];
        yield ['queue3', null];
        yield ['queue4', 'v3'];
    }

    #[Test, DataProvider('keysWithExpectedValues')]
    public function getNextQueuedValueByItsKey(string $key, ?string $expected): void
    {
        $this->client->queue('queue1', 'v1');
        $this->client->queue('queue2', 'v1');
        $this->client->queue('queue2', 'v2');
        $this->client->queue('queue3', 'v1');
        $this->client->queue('queue3', 'v1');
        $this->client->dequeue('queue3');
        $this->client->dequeue('queue3');
        $this->client->queue('queue4', 'v1');
        $this->client->queue('queue4', 'v2');
        $this->client->queue('queue4', 'v3');
        $this->client->dequeue('queue4');
        $this->client->dequeue('queue4');

        $actual = $this->client->get($key);

        self::assertEquals($expected, $actual);
    }

    #[Test]
    public function clear(): void
    {
        $this->client->queue('queue1', 'v1');
        $this->client->queue('queue1', 'v2');
        $this->client->queue('queue1', 'v3');

        $this->client->clear('queue1');

        self::assertNull($this->client->get('queue1'));
    }
}
