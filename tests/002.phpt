--TEST--
Check for linger_TrieTree presence
--SKIPIF--
<?php if (!extension_loaded("linger_TrieTree")) print "skip"; ?>
--FILE--
<?php 
$dic = './tmp.dic';
$words = array('管理员','admin','哈哈','我擦');
linger\TrieTree::build($words, $dic);

$filter = new linger\TrieTree($dic);
var_dump($filter);
$content = 'this is testadmin这是一段测试文字，哈哈的飞洒管理员的司法地方哈哈，火红的萨来开发大健康我擦';
$res = $filter->searchOne($content);
var_dump($res);
$res = $filter->searchAll($content);
print_r($res);
?>
--EXPECTF--
object(Linger\TrieTree)#1 (0) {
}
string(6) "admin"
Array
(
    [0] => admin
    [1] => 哈哈
    [2] => 管理员
    [3] => 哈哈
    [4] => 我擦
)