#!/usr/local/bin/php -q
<?php
error_reporting(E_ALL);
/* Allow the script to hang around waiting for connections. */
set_time_limit(0);
/* Turn on implicit output flushing so we see what we're getting as it comes in. */
ob_implicit_flush();


$LOCALHOST_IP = '127.0.0.1';
$LOCALHOST 	= gethostbyname('localhost');
$TEMP_FILE = "response.txt";
$RESPONSE_SERVER_FILE = "response_physical.srv";
$REQUEST_SERVER_FILE = "request_physical.srv";
$RESPONSE_CLIENT_FILE = "response_physical.cli";
$REQUEST_CLIENT_FILE = "request_physical.cli";
$INTERNET_PORT_SERVER = 21111;
$INTERNET_PORT_CLIENT = 21112;
$TRANSPORT_PORT_SERVER = 31111;
$TRANSPORT_PORT_CLIENT = 31112;
$MAX_BUF = 81920;


$listener = socket_create(AF_INET, SOCK_STREAM, SOL_TCP)
    or die ("creating socket failed! reason: " . socket_strerror(socket_last_error()) . "\n");

socket_bind($listener, $LOCALHOST_IP, $INTERNET_PORT_SERVER) 
    or die ("bind failed! reason: ".socket_strerror(socket_last_error($listener)) . "\n");

socket_listen($listener, 5)
    or die("listening failed! reason: " . socket_strerror(socket_last_error($listener)) . "\n");


do {
	echo "Internet Layer listening to port $INTERNET_PORT_SERVER...\n";
    $physicalSock = socket_accept($listener)
        or die ("socket_accept() failed: reason: " . socket_strerror(socket_last_error($listener)) . "\n");

    echo "Physical layer connection stabilished!\n";
    $buf = socket_read($physicalSock, $MAX_BUF)
        or die ("socket_read() failed: reason: " . socket_strerror(socket_last_error($msgsock)) . "\n");
    echo $buf;

    /* Reading request file made from the physical layer*/
    $file = fopen($REQUEST_SERVER_FILE, "r") or die("Unable to open request file!\n");
    $request = fread($file,filesize($REQUEST_SERVER_FILE));
    fclose($file);
	echo "Request received:\n\t$request\n";


    /* *************************************************************
        DO SOME INERNET LAYER PROCESSING HERE
    ************************************************************** */


	/* Connecting with transport layer */
	echo "Connecting with transport layer... ";
	$transportSock = socket_create(AF_INET, SOCK_STREAM, SOL_TCP)
        or die ("creating socket failed! reason: " . socket_strerror(socket_last_error()) . "\n");
	$result = socket_connect($transportSock, $LOCALHOST, $TRANSPORT_PORT_SERVER);
	if ($result === false) {
	    echo "socket connect with transport layer failed.\n";
	    echo "Reason: ($result) ".socket_strerror(socket_last_error($socket))."\n";
	} else {
	    echo "OK.\n";
	}
	echo "Sending UDP package to transport layer...\n";
	socket_write($transportSock, $request);


	/* Receives transport layer response */
	echo "Waiting to Transport Layer to deliver response...\n";
	if (false === ($buf = socket_read($transportSock, $MAX_BUF))) {
        echo "socket_read() failed: reason: " . socket_strerror(socket_last_error($msgsock)) . "\n";
        break;
    }
    echo "Transport Layer Response received: \n";
    echo $buf;


    /* **********
        DO SOME INERNET LAYER PROCESSING HERE
    ********** */

    // Write down response to a file that Physical layer can read
    //$file = fopen($RESPONSE_SERVER_FILE, "w") or die("Unable to open response file!\n");
    //$request = fwrite($file,$buf);
    //fclose($file);

    // send package to physical layer
	echo "\nSending package to physical layer...\n";
	socket_write($physicalSock, $buf);

    socket_close($transportSock);
    socket_close($physicalSock);
    echo "My work is done here! *Flies away*\n\n\n";
} while (true);

?>