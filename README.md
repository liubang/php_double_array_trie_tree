# php spam word filter based on Double-Array Trie tree

[![Build Status](https://travis-ci.org/liubang/php_double_array_trie_tree.svg?branch=master)](https://travis-ci.org/liubang/php_double_array_trie_tree)

## Dependence

- php >=5.4

## Useage

**Compile and Install**

```shell
git clone https://github.com/liubang/php-double-array-trie-tree.git
cd php-double-array-trie-tree
phpize
./configure
make && sudo make install
```

```php
<?php

$words = array('管理员','admin','哈哈','我擦');
linger\TrieTree::build($words, "./bbb.dic");

$filter = new linger\TrieTree('./bbb.dic');
$content = 'this is testadmin这是一段测试文字，哈哈的飞洒管理员的司法地方哈哈，火红的萨来开发大健康我擦';
$res = $filter->searchOne($content);
var_dump($res);
$res = $filter->searchAll($content);
print_r($res);
```

## Performance

- Size of dictionary words: 3954
- Length of destination string: 14352
- Times: 100000

```shell
liubang@venux:~/workspace/php/test/filter$ php test.php
Double-Array Trie tree: 1.86985206604
    regular expression: 63.114347934723
```

## Thanks

Inspired by [wulijun/php-ext-trie-filter](https://github.com/wulijun/php-ext-trie-filter.git)

Depends on [libdatrie-0.2.4](https://linux.thai.net/~thep/datrie/datrie.html)
