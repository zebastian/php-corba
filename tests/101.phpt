--TEST--
Call functions
--SKIPIF--
<?php if (!extension_loaded("universe")) print "skip"; ?>
--POST--
--GET--
--FILE--
<?php
// vim: syntax=php
class MyRandom
{
	function lrand48()
	{
		echo "lrand48\n";
		return 0;
	}

	function mrand48()
	{
		echo "mrand48\n";
		return 0;
	}
}

$server = new MyRandom();
$object = new UniverseObject($server, "IDL:Random:1.0");

$l = $object->lrand48();
$m = $object->mrand48();

?>
--EXPECT--
lrand48
mrand48
