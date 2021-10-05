--TEST--
Output parameters
--SKIPIF--
<?php if (!extension_loaded("universe")) print "skip"; ?>
--POST--
--GET--
--FILE--
<?php
// vim: syntax=php
class OutputParameters
{
	function get_string($value)
	{
		$value = "nisse";
	}

	function get_long($value)
	{
		$value = 222;
	}
}

$server = new OutputParameters();
$object = new UniverseObject($server, "IDL:OutputParameters:1.0");

$object->get_string(&$value);
echo "$value\n";

$object->get_long(&$value);
echo "$value\n";


?>
--EXPECT--
nisse
222
