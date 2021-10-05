--TEST--
Check return value from functions
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
		return 10;
	}

	function mrand48()
	{
		echo "mrand48\n";
		return 20;
	}
}

$server = new MyRandom();
$object = new UniverseObject($server, "IDL:Random:1.0");

$l = $object->lrand48();
echo "$l\n";

$m = $object->mrand48();
echo "$m\n";

?>
--EXPECT--
lrand48
10
mrand48
20
