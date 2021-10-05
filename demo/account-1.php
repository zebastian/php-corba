<?php

/*
	PHP implementation of mico/demo/poa/account-1/client.cc
	                  and mico/demo/poa/account-2/client.cc
*/

/*
	 Connect to the Bank
*/

$path = getcwd();
$bank = new UniverseObject("file://$path/Bank.ref");

/*
	 Open an account
*/

$account = $bank->create();

/*
	 Deposit and withdraw some money
*/

$account->deposit(700);
$account->withdraw(450);

echo "Balance is ".$account->balance().".\n";

?>
