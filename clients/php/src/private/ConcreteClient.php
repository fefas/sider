<?php

namespace Sider\Client;

use Sider\Client;

final readonly class ConcreteClient implements Client
{
    public function __construct(
        private SocketConnection $connection,
    ) {
    }

    public function set(string $key, string $value): void
    {
        $this->connection->send("SET $key $value;");
        $this->connection->receive();
    }

    public function get(string $key): string
    {
        $this->connection->send("GET $key;");
        $response = $this->connection->receive();

        return $response;
    }

    public function disconnect(): void
    {
        $this->connection->close();
    }
}
