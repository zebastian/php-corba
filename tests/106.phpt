--TEST--
Sequences as parameters
--SKIPIF--
<?php if (!extension_loaded("universe")) print "skip"; ?>
--POST--
--GET--
--FILE--
<?php
// vim: syntax=php

class SequenceParameter
{
	var $values;

	function set_values($values)
	{
		var_dump($values);
		$this->values = $values;
	}

	function get_values()
	{
		return array(40,50,60,70);
	}
}

$demo = new UniverseObject(new SequenceParameter(), "IDL:SequenceParameter:1.0");

$values = array(10,20,30);
var_dump($values);

$demo->set_values($values);

$values2 = $demo->get_values();
var_dump($values2);
	
?>
--EXPECT--
array(3) {
  [0]=>
  int(10)
  [1]=>
  int(20)
  [2]=>
  int(30)
}
array(3) {
  [0]=>
  int(10)
  [1]=>
  int(20)
  [2]=>
  int(30)
}
array(4) {
  [0]=>
  int(40)
  [1]=>
  int(50)
  [2]=>
  int(60)
  [3]=>
  int(70)
}
