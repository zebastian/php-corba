--TEST--
Test universe_has_interface()
--SKIPIF--
<?php if (!extension_loaded("universe")) print "skip"; ?>
--POST--
--GET--
--FILE--
<?php
// vim: syntax=php

// This test will fail if you don't have Random loaded in your interface repository

echo universe_has_interface("IDL:Random:1.0") ? "yes" : "no";
echo "\n";
echo universe_has_interface("IDL:NotSoRandom:1.0") ? "yes" : "no";
echo "\n";

?>
--EXPECT--
yes
no
