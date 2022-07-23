<?php

/** @generate-legacy-arginfo */

class Linger_TrieTree
{
    public static function build(array $dict, string $path): bool;

    public function __construct(string $path);

    public function searchOne(string $content): string|false;

    public function searchAll(string $content): array|false;
}
