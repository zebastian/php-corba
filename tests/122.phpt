--TEST--
Call functions
--SKIPIF--
<?php /*if (!extension_loaded("universe"))*/ print "skip"; ?>
--POST--
--GET--
--FILE--
<?php
// vim: syntax=php
class MyAttributes
{
	var $short_value;

	function _get_short_value()
	{
		return $this->short_value;
	}

	function _put_short_value($value)
	{
		$this->short_value = $value;
	}


}

$server = new MyAttributes();
$object = new UniverseObject($server, "IDL:Attributes:1.0");

$object->short_value = 222;

echo $object->short_value."\n";

?>
--EXPECT--
222
