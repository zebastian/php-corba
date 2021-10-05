--TEST--
Exception handling
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
		$e = new UniverseStruct("IDL:dactylis/CertNotFound:1.0");
		universe_set_exception($e);
		return "Hepp\n";
	}
}

$server = new UniverseObject(new CertificateStore(), "IDL:dactylis/CertificateStore:1.0");

function errhnd($errno, $errmsg) {
  print "ERROR: $errno $errmsg\n";
}

set_error_handler('errhnd');
error_reporting(E_ALL);
print $server->getEncCert("zazaza");
	
?>
--EXPECT--
ERROR: 2 User exception: IDL:dactylis/CertNotFound:1.0
