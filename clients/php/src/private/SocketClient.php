<?php

namespace Sider\Client;

use Sider\Client;
use Sider\Rate;

final readonly class SocketClient implements Client
{
    public function __construct(
        private SocketConnection $connection,
        private string $scope,
    ) {
    }

    public function get(string $key, int $partition = 0): ?string
    {
        $this->connection->send(Command::get($this->scope, $key, $partition));

        $response = $this->connection->receive();

        return $response === 'nil' ? null : $response;
    }

    public function clear(string $key): void
    {
        $this->send(Command::clear($this->scope, $key));
    }

    public function keep(string $key, string $value, ?int $ttl = null): void
    {
        $this->send(Command::keep($this->scope, $key, $value, $ttl));
    }

    public function count(string $key, int $step = 1, ?int $ttl = null): void
    {
        $this->send(Command::count($this->scope, $key, $step, $ttl));
    }

    public function queue(string $key, string $value): void
    {
        $this->send(Command::queue($this->scope, $key, $value));
    }

    public function dequeue(string $key): void
    {
        $this->send(Command::dequeue($this->scope, $key));
    }

    public function rate(string $key, int $partition, int $step = 1, ?int $ttl = null): void
    {
        $this->send(Command::rate($this->scope, $key, $partition, $step, $ttl));
    }

    private function send(Command $command): string
    {
        $this->connection->send($command);

        return $this->connection->receive();
    }

    public function disconnect(): void
    {
        // $this->connection->close();
    }
}
