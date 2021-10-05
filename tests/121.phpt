--TEST--
Exception handling with non-empty exception
--SKIPIF--
<?php if (!extension_loaded("universe")) print "skip"; ?>
--POST--
--GET--
--FILE--
<?php
// vim: syntax=php

class CertificateStore
{
	function getEncCert()
	{
		$e = new UniverseStruct("IDL:dactylis2/CertNotFound:1.0");
		$e->message = "Something went wrong";
		universe_set_exception($e);
		return "Hepp\n";
	}
}

$server = new UniverseObject(new CertificateStore(), "IDL:dactylis2/CertificateStore:1.0");

function errhnd($errno, $errmsg) {
  print "ERROR: $errno $errmsg\n";
}

set_error_handler('errhnd');
error_reporting(E_ALL);
print $server->getEncCert("zazaza");
	
?>
--EXPECT--
ERROR: 2 User exception: IDL:dactylis2/CertNotFound:1.0
