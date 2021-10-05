--TEST--
Input parameters
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

$object->print_string("nisse");
$object->print_long(222);

?>
--EXPECT--
nisse
222
