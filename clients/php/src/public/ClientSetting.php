<?php

namespace Sider;

use Sider\Client\ConcreteClient;
use Sider\Client\SocketConnection;

final readonly class ClientSetting
{
    private function __construct(
        private string $host,
        private int $port,
    ) {
    }

    public static function withUrl(string $url): self
    {
        $parsedUrl = parse_url($url);

        return new self($parsedUrl['host'], $parsedUrl['port']);
    }

    public function build(): Client
    {
        return new ConcreteClient(new SocketConnection($this->host, $this->port));
    }
}
