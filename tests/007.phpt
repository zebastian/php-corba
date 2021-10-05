--TEST--
Invalid IOR
--SKIPIF--
<?php if (!extension_loaded("universe")) print "skip"; ?>
--POST--
--GET--
--FILE--
<?php
$ior = "blablabla";
$obj = @universe_string_to_object($ior);
echo "is_object=".is_object($obj)."\n";
?>
--EXPECT--
is_object=
