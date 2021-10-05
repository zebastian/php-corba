--TEST--
Named enum members
--SKIPIF--
<?php if (!extension_loaded("universe")) print "skip"; ?>
--POST--
--GET--
--FILE--
<?php
// vim: syntax=php

class DataTypes 
{
	function loop_enum($value) { return $value; }
}

$object = new UniverseObject(new DataTypes(), "IDL:DataTypes:1.0");

echo $object->loop_enum(1)."\n";
echo $object->loop_enum(two)."\n";
echo $object->loop_enum("three")."\n";


?>
--EXPECT--
one
two
three
