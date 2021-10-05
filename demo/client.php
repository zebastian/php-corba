<?php

/*
	Call a CORBA server that implements the InputParameters interface and
  stores the IOR in server.ior.

	See server.php for an example server for this client.
*/

$path = getcwd();
$server = new UniverseObject("file://$path/server.ior");

$server->print_string($argv[1]);
$server->print_long($argv[2]);

?>
