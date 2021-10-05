<?php

/*
	PHP implementation of mico/demo/poa/account-3/client.cc

	Start with the account-3 shell script
*/

/*
	 Connect to the Bank
*/

$path = getcwd();
$bank = new UniverseObject("file://$path/Bank.ref");

/*
	 Open an account
*/

$account = $bank->create("David");

$bank->shutdown();	// explicit server shutdown

/*
	 Deposit and withdraw some money
*/

$account->deposit(700);
$bank->shutdown();

$account->withdraw(450);
$bank->shutdown();

echo "Balance is ".$account->balance().".\n";
$bank->shutdown();

?>
