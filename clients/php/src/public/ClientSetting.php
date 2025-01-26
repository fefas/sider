<?php

namespace Sider;

use Sider\Client\SocketClient;
use Sider\Client\SocketConnection;

final readonly class ClientSetting
{
    private function __construct(
        private string $host,
        private int $port,
        private string $scope,
    ) {
    }

    public static function withUrl(string $url): self
    {
        $parsedUrl = parse_url($url);

        return new self(
            host: $parsedUrl['host'],
            port: $parsedUrl['port'],
            scope: '',
        );
    }

    public function withScope(string $scope): self
    {
        return new self($this->host, $this->port, $scope);
    }

    public function build(): Client
    {
        return new SocketClient(
            new SocketConnection($this->host, $this->port),
            $this->scope,
        );
    }
}
