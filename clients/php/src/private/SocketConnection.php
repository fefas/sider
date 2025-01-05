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
        $this->open();
    }

    public function open(): void
    {
        socket_connect($this->socket, $this->host, $this->port);
    }

    public function close(): void
    {
        socket_close(socket: $this->socket);
    }

    public function send(Command $command): void
    {
        $data = $command->toBytes();
        
        socket_write(socket: $this->socket, data: $data, length: strlen($data));

        // var_dump("Sent command: " . $command->toReadableHex() . " length: " . strlen($command->toBytes()));
    }

    public function receive(): string
    {
        $response = socket_read(socket: $this->socket, length: 1024);

        // var_dump("Received response: " . $response);

        return $response;
    }
}
