--TEST--
Passing an object
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

class ObjectParameter
{
	var $object;

	function set_object($value)
	{
		$this->object = $value;
	}

	function get_object()
	{
		return $this->object;
	}
}

$object = new UniverseObject(new InputParameters(), "IDL:InputParameters:1.0");
$demo = new UniverseObject(new ObjectParameter(), "IDL:ObjectParameter:1.0");

$demo->set_object($object);

$object->print_string("nisse");
$object->print_long(222);

$object2 = $demo->get_object();

$object2->print_string("kalle");
$object2->print_long(444);

?>
--EXPECT--
nisse
222
kalle
444
