--TEST--
Verify dictionary build and keyword matching workflow
--SKIPIF--
<?php if (!extension_loaded("linger_TrieTree")) print "skip"; ?>
--FILE--
<?php
$dictionaryFile = './tmp.dic';
$dictionary = array(
    '管理员',
    'administrator',
    '敏感词',
    'restricted term',
    'internal use only'
);
Linger\TrieTree::build($dictionary, $dictionaryFile);

$trie = new Linger\TrieTree($dictionaryFile);
var_dump($trie);

$input = 'This document is marked for internal use only. 请立即通知管理员，因为本段内容包含敏感词和 restricted term。';

$firstMatch = $trie->searchOne($input);
var_dump($firstMatch);

$allMatches = $trie->searchAll($input);
print_r($allMatches);
?>
--EXPECTF--
object(Linger\TrieTree)#1 (0) {
}
string(17) "internal use only"
Array
(
    [0] => internal use only
    [1] => 管理员
    [2] => 敏感词
    [3] => restricted term
)
