<?php

$path = getcwd();
$client = new UniverseObject("file://$path/struct.ref");

/*
	create structure
*/
$st1 = new UniverseStruct("IDL:S:1.0");

/*
	fill structure
*/
$st1->a = 47;
$st1->b = 11;
$st1->c = ord('S');

/*
	when this call is made, the server should output:

  47
  11
  S
*/ 
$client->bar( $st1, &$st2 );

/*
	these lines should output:

  47
  11
  S
*/
echo $st2->a."\n";
echo $st2->b."\n";
echo chr($st2->c)."\n\n";

$s = new UniverseStruct("IDL:S2:1.0");

$s->a = 4711;
$s->x = $st1;

/*
	when this call is made, the server should output:

  4711
  47
  11
  S
*/
$client->bar2( $s );

$s2 = $client->bar3();

/*
	these lines should output:

	42
  P
*/
echo $s2->a."\n";
/*
$x = $s2->x;
echo chr($x->c)."\n";		// why does not $s2->x->c work?

echo "Not working:\n";
flush();*/
echo chr($s2->x->c)."\n";

?>
