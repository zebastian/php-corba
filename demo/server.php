<?php

/*
	Create a CORBA server in PHP

	Requires InputParameters.idl loaded in the interface repository.

	See client.php for example client for this server.
*/

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

$object = new UniverseObject(new InputParameters(), "IDL:InputParameters:1.0");

$object->print_string("Server started");

$file = fopen("server.ior", "w");
fwrite($file, universe_object_to_string($object));
fclose($file);

universe_run();

?>
