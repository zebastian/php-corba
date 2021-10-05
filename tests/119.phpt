--TEST--
Sequences of octets becomes strings
--SKIPIF--
<?php if (!extension_loaded("universe")) print "skip"; ?>
--POST--
--GET--
--FILE--
<?php
// vim: syntax=php

class OctetSequenceTest
{
	var $values;

	function set_values($values)
	{
		var_dump($values);
		$this->values = $values;
	}

	function get_values()
	{
		return strrev($this->values);
	}
}

$demo = new UniverseObject(new OctetSequenceTest(), "IDL:OctetSequenceTest:1.0");

$values = "2GooD";
var_dump($values);

$demo->set_values($values);

$values2 = $demo->get_values();
var_dump($values2);
	
?>
--EXPECT--
string(5) "2GooD"
string(5) "2GooD"
string(5) "DooG2"
