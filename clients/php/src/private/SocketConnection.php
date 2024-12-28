<?php

namespace Sider\Client;

use Socket;
use socket_close;
use socket_connect;
use socket_create;
use socket_read;
use socket_write;

final readonly class SocketConnection
{
    private Socket $socket;

    public function __construct(
        private string $host,
        private int $port,
    ) {
        $this->socket = socket_create(domain: AF_INET, type: SOCK_STREAM, protocol: SOL_TCP);
        socket_connect($this->socket, $this->host, $this->port);
    }

    public function close(): void
    {
        socket_close(socket: $this->socket);
    }

    public function send(string $data): void
    {
        socket_write(socket: $this->socket, data: $data);
    }

    public function receive(): string
    {
        return socket_read(socket: $this->socket, length: 10);
    }
}
