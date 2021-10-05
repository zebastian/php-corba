--TEST--
Bounded strings
--SKIPIF--
<?php if (!extension_loaded("universe")) print "skip"; ?>
--POST--
--GET--
--FILE--
<?php
// vim: syntax=php

/*
	 Load IDL like this:

	 idl -ORBIfaceRepoAddr inet:localhost:2222 --feed-ir bik.idl
*/

class BikboekImpl
{
	function getCategorie($c)
	{
		echo "$c\n";
		$result = new UniverseStruct("IDL:nederland.net/BIK/Categorie:1.0");
		$result->id = "someid";
		$result->naam = "somenaam";
		return $result;
	}
};

    $myObj = new UniverseObject(new BikboekImpl(), "IDL:nederland.net/BIK/bikboek:1.0");

    $res = $myObj->getCategorie("A");                

		echo $res->id."\n";
		echo $res->naam."\n";
?>
--EXPECT--
A
someid
somenaam
