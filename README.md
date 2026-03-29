# PHP Double-Array Trie Tree Extension

![Build Status](https://github.com/liubang/php_double_array_trie_tree/workflows/integrate/badge.svg?branch=master)
![License: MIT](https://img.shields.io/github/license/liubang/php_double_array_trie_tree?style=flat-square)
![GitHub release (latest by date)](https://img.shields.io/github/v/release/liubang/php_double_array_trie_tree?style=flat-square)

`php_double_array_trie_tree` is a native PHP extension that provides high-performance keyword matching based on a Double-Array Trie (DAT) index.

The extension is designed for workloads such as sensitive-word filtering, keyword detection, lexical lookup, and other string matching scenarios where deterministic lookup latency and low memory overhead are preferred over regular-expression-based scanning.

Internally, the project embeds a `libdatrie`-style implementation and exposes a compact PHP API for:

- offline dictionary compilation into a serialized trie file
- low-latency loading of the compiled dictionary
- first-hit lookup
- full-hit enumeration across an input string

## Why Double-Array Trie

A Double-Array Trie is a trie representation optimized for compactness and traversal efficiency. Compared with naive hash-based keyword scans or large regular-expression alternations, DAT-based matching typically offers:

- predictable traversal semantics
- efficient prefix-state transitions
- reduced runtime overhead for large keyword sets
- good suitability for repeated lookups against a precompiled dictionary

This extension is therefore a strong fit for read-heavy filtering pipelines where the dictionary is built once and queried many times.

## Feature Set

- Native PHP extension implemented in C
- Dictionary compilation to a reusable on-disk trie artifact
- Fast lookup through a serialized double-array trie
- `searchOne()` for first-match retrieval
- `searchAll()` for exhaustive match enumeration
- Support for repeated matches in left-to-right scan order
- Bundled trie implementation, with no external runtime dependency required during build

## Architecture Overview

The extension follows a two-phase workflow:

1. Build phase: a PHP array of keywords is compiled into a binary trie file via `Linger\TrieTree::build()`.
2. Query phase: the trie file is loaded by the constructor and reused for matching operations.

At initialization time, the trie alphabet is configured as the full byte range `0x00-0xff`, which makes the implementation byte-oriented rather than language-specific. In practice, this allows the extension to process arbitrary byte sequences, including UTF-8 encoded text, as long as the dictionary and the input content use the same encoding.

## Requirements

- PHP 5.4 or later
- `phpize`
- a C compiler toolchain compatible with PHP extension builds

The source tree contains compatibility branches for legacy PHP 5 as well as modern PHP runtimes.

## Installation

Clone the repository and build the extension in the standard PHP extension toolchain:

```bash
git clone https://github.com/liubang/php_double_array_trie_tree.git
cd php_double_array_trie_tree
phpize
./configure --enable-linger_TrieTree
make
sudo make install
```

Enable the extension in `php.ini`:

```ini
extension=linger_TrieTree.so
```

To verify that the module is loaded:

```bash
php -m | grep linger_TrieTree
```

## Public API

The extension exposes the class `Linger\TrieTree`.

### `Linger\TrieTree::build(array $dict, string $path): bool`

Builds a trie from the provided dictionary and serializes it to `path`.

Parameters:

- `dict`: list of keywords to be indexed
- `path`: output path for the serialized dictionary file

Returns:

- `true` on success
- `false` on build failure

### `new Linger\TrieTree(string $path)`

Loads a previously compiled trie from disk.

Parameters:

- `path`: path to the serialized trie file

Behavior:

- throws an exception if the file cannot be loaded

### `searchOne(string $content): ?string`

Scans the input from left to right and returns the first matched keyword encountered during traversal.

Behavior:

- returns the first matched term as a string
- returns `null` when no match is found
- throws an exception when `content` is empty

### `searchAll(string $content): array`

Scans the input from left to right and returns all matched keywords in encounter order.

Behavior:

- returns an array of matched terms
- preserves duplicates when the same keyword appears multiple times
- returns an empty array when no match is found
- throws an exception when `content` is empty

## Usage Example

The following example demonstrates a typical moderation-oriented workflow: compile a keyword dictionary once, load the serialized trie, and then execute both first-hit and full-hit matching against an input document.

```php
<?php

$dictionary = [
    '管理员',
    'administrator',
    '敏感词',
    'restricted term',
    'internal use only',
];
$dictionaryFile = __DIR__ . '/keywords.dic';

Linger\TrieTree::build($dictionary, $dictionaryFile);

$trie = new Linger\TrieTree($dictionaryFile);

$input = 'This document is marked for internal use only. 请立即通知管理员，因为本段内容包含敏感词和 restricted term。';

$firstMatch = $trie->searchOne($input);
var_dump($firstMatch);

$allMatches = $trie->searchAll($input);
print_r($allMatches);
```

In this example:

- `searchOne()` returns the earliest detected keyword in scan order
- `searchAll()` returns every detected keyword, including repeated occurrences

Representative output:

```text
string(17) "internal use only"
Array
(
    [0] => internal use only
    [1] => 管理员
    [2] => 敏感词
    [3] => restricted term
)
```

## Matching Semantics

- Matching is performed over bytes, not Unicode code points.
- `searchOne()` returns the first successful terminal match discovered during the scan.
- `searchAll()` enumerates matches in left-to-right order and keeps repeated hits.
- The dictionary should be built with the same character encoding used by the target content.

For multilingual text, UTF-8 is typically the safest choice as long as both dictionary entries and input strings are consistently UTF-8 encoded.

## Performance Notes

The original benchmark included in this project reports the following indicative result:

- Dictionary size: `3954` entries
- Input length: `14352` characters
- Iterations: `100000`

```text
Double-Array Trie tree: 1.86985206604
regular expression:    63.114347934723
```

Actual performance will depend on:

- PHP version
- compiler flags
- CPU architecture
- dictionary cardinality and keyword distribution
- input size and match density

Even so, the extension is clearly intended for scenarios where precompiled trie traversal materially outperforms repeated regular-expression evaluation.

## Development and Test

Build locally:

```bash
phpize
./configure --enable-linger_TrieTree
make clean
make
```

Run the bundled PHPT test suite:

```bash
make test
```

## Repository Layout

- `linger_TrieTree.c`: PHP extension entry points and exposed methods
- `php_linger_TrieTree.h`: extension-level declarations and compatibility macros
- `src/datrie/`: embedded double-array trie implementation
- `tests/`: PHPT regression tests
- `config.m4` / `config.w32`: Unix and Windows extension build configuration

## Use Cases

- sensitive-word filtering
- moderation pipelines
- blacklist or denylist matching
- keyword-trigger routing
- exact-term lexical detection in high-throughput services

## Acknowledgements

- Inspired by [wulijun/php-ext-trie-filter](https://github.com/wulijun/php-ext-trie-filter)
- Built on an embedded [libdatrie](https://linux.thai.net/~thep/datrie/datrie.html)-style trie implementation

## License

This project is released under the [MIT License](LICENSE).
