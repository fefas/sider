<?php

namespace Sider\Client;

use Sider\Client;
use Sider\Rate;

final readonly class ConcreteClient implements Client
{
    public function __construct(
        private SocketConnection $connection,
    ) {
    }

    public function set(string $key, string $value, ?string $ttl = null): void
    {
        $this->connection->send(match ($ttl) {
            null => "STORE $key AS $value;",
            default => "STORE $key AS $value TTL $ttl;"
        });

        $this->connection->receive();
    }

    public function count(string $key): void
    {
        $this->connection->send("COUNT $key;");
        $this->connection->receive();
    }

    public function rate(string $key, Rate $rate): void
    {
        $this->connection->send(match ($rate) {
            Rate::TOTAL => "RATE $key TOTAL;",
            Rate::PART => "RATE $key PART;",
        });

        $this->connection->receive();
    }

    public function get(string $key): ?string
    {
        $this->connection->send("GET $key;");

        $response = $this->connection->receive();

        return $response === 'null' ? null : $response;
    }

    public function disconnect(): void
    {
        $this->connection->close();
    }
}
