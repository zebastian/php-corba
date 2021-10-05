--TEST--
universe_string_to_object()
--SKIPIF--
<?php if (!extension_loaded("universe")) print "skip"; ?>
--POST--
--GET--
--FILE--
<?php
$ior = "IOR:000000000000000f49444c3a52616e646f6d3a312e3000000000000100000000000000500001000000000016706c616e7874792e6473672e63732e7463642e69650006220000002c3a5c706c616e7874792e6473672e63732e7463642e69653a52616e646f6d3a303a3a49523a52616e646f6d00";
$obj = universe_string_to_object($ior);
echo "is_object=".is_object($obj)."\n";
?>
--EXPECT--
is_object=1
