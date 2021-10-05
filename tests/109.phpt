--TEST--
Lots of data types
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

class DataTypes 
{
	function loop_short($value) { return $value; }
	function loop_long($value) { return $value; }
	function loop_ushort($value) { return $value; }
	function loop_ulong($value) { return $value; }
	function loop_float($value) { return $value; }
	function loop_double($value) { return $value; }
	function loop_boolean($value) { return $value; }
	function loop_char($value) { return $value; }
	function loop_octet($value) { return $value; }
	function loop_object($value) { return $value; }
	function loop_struct($value) { return $value; }
	function loop_string($value) { return $value; }
	function loop_long_seq($value) { return $value; }
	function loop_enum($value) { return $value; }
}

$object = new UniverseObject(new DataTypes(), "IDL:DataTypes:1.0");

echo $object->loop_short(1)."\n";
echo $object->loop_long(2)."\n";
echo $object->loop_ushort(3)."\n";
echo $object->loop_ulong(4)."\n";
echo $object->loop_float(5.5)."\n";
echo $object->loop_double(6.6)."\n";
echo $object->loop_boolean(1)."\n";
echo $object->loop_char(7)."\n";
echo $object->loop_octet(8)."\n";
echo $object->loop_string("wow")."\n";
echo $object->loop_enum(0)."\n";

$value = new UniverseObject(new object_data_type(), "IDL:object_data_type:1.0");
$value2 = $object->loop_object($value);
$value2->print_long(222);

$value = new UniverseStruct("IDL:struct_data_type:1.0");
$value->member = 333;
$value2 = $object->loop_struct($value);
echo $value2->member."\n";

$value = array(10,20,30);
$value2 = $object->loop_long_seq($value);
var_dump($value2);

?>
--EXPECT--
1
2
3
4
5.5
6.6
1
7
8
wow
zero
222
333
array(3) {
  [0]=>
  int(10)
  [1]=>
  int(20)
  [2]=>
  int(30)
}
