<?php

namespace Sider\Client;

final readonly class Command
{
    private const PROTOCOL_VERSION = 1;
    
    // checksum: 4 bytes
    private const CHECKSUM_FORMAT = 'V';

    // protocol version: 2 bytes
    // content type: 2 bytes
    // content length: 4 bytes
    private const HEADER_FORMAT = 'vvV';

    // scope length: 2 byte
    // key length: 2 byte
    // scope: N bytes
    // key: N bytes
    private const CONTENT_BASE_FORMAT = 'vva*a*';

    private const CLEAR = 20;
    private const GET = 21;
    private const KEEP = 22;
    private const COUNT = 23;
    private const RATE = 24;
    private const QUEUE = 30;
    private const DEQUEUE = 31;

    private int $contentType;
    private string $contentFormat;
    private array $contentValues;
    
    private function __construct(int $type, string $scope, string $key, string $argsFormat, array $argsValues)
    {
        $this->contentType = $type;

        $this->contentFormat = self::CONTENT_BASE_FORMAT . $argsFormat;
        $this->contentValues = [strlen($scope), strlen($key), $scope, $key, ...$argsValues];
    }

    public static function get(string $scope, string $key, int $partition = 0): self
    {
        return new self(    
            type: self::GET,
            scope: $scope,
            key: $key,
            argsFormat: 'C',
            argsValues: [$partition],
        );
    }

    public static function clear(string $scope, string $key): self
    {
        return new self(
            type: self::CLEAR,
            scope: $scope,
            key: $key,
            argsFormat: '',
            argsValues: [],
        );
    }

    public static function keep(string $scope, string $key, string $value, ?int $ttl): self
    {
        return new self(    
            type: self::KEEP,
            scope: $scope,
            key: $key,
            argsFormat: 'Vva*',
            argsValues: [$ttl ?? 0, strlen($value), $value],
        );
    }

    public static function queue(string $scope, string $key, string $value): self
    {
        return new self(
            type: self::QUEUE,
            scope: $scope,
            key: $key,
            argsFormat: 'va*',
            argsValues: [strlen($value), $value],
        );
    }

    public static function dequeue(string $scope, string $key): self
    {
        return new self(
            type: self::DEQUEUE,
            scope: $scope,
            key: $key,
            argsFormat: '',
            argsValues: [],
        );
    }

    public static function count(string $scope, string $key, int $step, ?int $ttl): self
    {
        return new self(    
            type: self::COUNT,
            scope: $scope,
            key: $key,
            argsFormat: 'CV',
            argsValues: [$step, $ttl],
        );
    }

    public static function rate(string $scope, string $key, int $partition, int $step = 1, ?int $ttl): self
    {
        return new self(
            type: self::RATE,
            scope: $scope,
            key: $key,
            argsFormat: 'CCV',
            argsValues: [$partition, $step, $ttl],
        );
    }

    public function toBytes(): string
    {
        $content = pack($this->contentFormat, ...$this->contentValues);
        $header = pack(self::HEADER_FORMAT, self::PROTOCOL_VERSION, $this->contentType, strlen($content));
        $checksum = pack(self::CHECKSUM_FORMAT, crc32($header . $content));

        return $checksum . $header . $content;
    }

    public function toReadableHex(): string
    {
        return implode(' ', str_split(bin2hex($this->toBytes()), 2));
    }
}
