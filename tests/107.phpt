--TEST--
Serialization of UniverseObject
--SKIPIF--
<?php if (!extension_loaded("universe")) print "skip"; ?>
--POST--
--GET--
--FILE--
<?php
// vim: syntax=php
class InputParameters
{
	function print_string($value)
	{
		echo "$value\n";
	}

	function print_long($value)
	{
		echo "$value\n";
	}
}

$server = new InputParameters();
$object = new UniverseObject($server, "IDL:InputParameters:1.0");

$buffer = serialize($object);
$object2 = unserialize($buffer);

$object2->print_string("nisse");
$object2->print_long(222);

?>
--EXPECT--
nisse
222
