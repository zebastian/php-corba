--TEST--
Array in struct (works when arrays are implemented :-)
--SKIPIF--
<?php if (!extension_loaded("universe")) print "skip"; ?>
--POST--
--GET--
--FILE--
<?php
// vim: syntax=php



class object_data_type
{
	function print_long($value)
	{
		echo "$value\n";
	}	
}

class ExampleInterface
{
	function ReturnTheNumberFive() { return 5; }
	function ExampleMethod($key, $e_struct)
	{ 
		$e_struct->smember = 1;
		$e_struct->lmember = 2;
		$e_struct->cmember = 3;
		$e_struct->camember = 4;

		return 5;
	}
}

$object = new UniverseObject(new ExampleInterface(), "IDL:mmod/ExampleInterface:1.0");

$mmod = new UniverseStruct("IDL:mmod/ExampleStruct:1.0");
echo $object->ReturnTheNumberFive()."\n"; // Works
echo $object->ExampleMethod(1, $mmod)."\n"; // Doesn't work. Line 10

?>
--EXPECT--
5
5
