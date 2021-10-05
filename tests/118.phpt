--TEST--
Fixed-point types
--SKIPIF--
<?php /*if (!extension_loaded("universe"))*/ print "skip"; ?>
--POST--
--GET--
--FILE--
<?php
// vim: syntax=php

/*
	 Load IDL like this:

	 idl -ORBIfaceRepoAddr inet:localhost:2222 --feed-ir fixed.idl
*/

class TestFixedImpl
{
	function multiply($x, $y)
	{
		echo "x=$x\n";
		echo "y=$y\n";
		return bcmul($x, $y);
	}
};

    $myObj = new UniverseObject(new TestFixedImpl(), "IDL:TestFixed:1.0");

    $value = $myObj->multiply(
				"00000000.01", 
				"1234567.000");

		echo "r=$value\n";
?>
--EXPECT--
x=00000000.01
y=1234567.000
r=12345.67000
