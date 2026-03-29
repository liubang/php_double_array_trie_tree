--TEST--
Verify that the linger_TrieTree extension is loaded
--SKIPIF--
<?php if (!extension_loaded("linger_TrieTree")) print "skip"; ?>
--FILE--
<?php
echo "linger_TrieTree extension is available";
?>
--EXPECT--
linger_TrieTree extension is available
