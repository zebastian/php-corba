--TEST--
Serialization of UniverseStruct
--SKIPIF--
<?php if (!extension_loaded("universe")) print "skip"; ?>
--POST--
--GET--
--FILE--
<?php
// vim: syntax=php

$struct = new UniverseStruct("IDL:struct_data_type:1.0");

$struct->member = 222;
echo $struct->member."\n";

$buffer = serialize($struct);
echo $struct->member."\n";

$struct2 = unserialize($buffer);

echo $struct2->member."\n";

?>
--EXPECT--
222
222
222
