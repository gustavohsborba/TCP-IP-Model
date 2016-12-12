#!/usr/local/bin/php -q
<?php
error_reporting(E_ALL);
/* Allow the script to hang around waiting for connections. */
set_time_limit(0);
/* Turn on implicit output flushing so we see what we're getting as it comes in. */
ob_implicit_flush();


$LOCALHOST_IP = '127.0.0.1';
$LOCALHOST = gethostbyname('localhost');
$TEMP_FILE     "response.txt"
$RESPONSE_FILE "response_physical.tmp"
$REQUEST_FILE  "request_physical.tmp"
$INTERNET_PORT_SERVER = 63051;
$INTERNET_PORT_CLIENT = 63041;
$TRANSPORT_PORT_SERVER = 63052;
$TRANSPORT_PORT_CLIENT = 63042;
$MAX_BUF = 81920


if (($listener = socket_create(AF_INET, SOCK_STREAM, SOL_TCP)) === false) {
    echo "socket_create() failed: reason: " . socket_strerror(socket_last_error()) . "\n";
}

if (socket_bind($listener, $LOCALHOST_IP, $INTERNET_PORT_SERVER) === false) {
    echo "socket_bind() failed: reason: " . socket_strerror(socket_last_error($listener)) . "\n";
}

if (socket_listen($listener, 5) === false) {
    echo "socket_listen() failed: reason: " . socket_strerror(socket_last_error($listener)) . "\n";
}


do {
	echo "Internet Layer listening to port $INTERNET_PORT_SERVER...";
    if (($physicalSock = socket_accept($listener)) === false) {
        echo "socket_accept() failed: reason: " . socket_strerror(socket_last_error($listener)) . "\n";
        break;
    }

    echo "Physical layer connection stabilished!";
    if (false === ($buf = socket_read($physicalSock, $MAX_BUF, PHP_NORMAL_READ))) {
        echo "socket_read() failed: reason: " . socket_strerror(socket_last_error($msgsock)) . "\n";
        break 2;
    }
    echo $buf;

    /* Reading request file made from the physical layer*/
    $file = fopen(REQUEST_FILE, "r") or die("Unable to open request file!");
    $request = fread($file,filesize(REQUEST_FILE));
    fclose($file);
	echo "Request received:" . $request;

	/* Connecting with transport layer */
	echo "Connecting with transport layer...";
	$transportSock = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
	$result = socket_connect($transportSock, $LOCALHOST, $TRANSPORT_PORT_SERVER);
	if ($result === false) {
	    echo "socket connect with transport layer failed.";
	    echo "Reason: ($result) ".socket_strerror(socket_last_error($socket))."\n";
	} else {
	    echo "OK.\n";
	}
	echo "Sending UDP package to transport layer...";
	socket_write($transportSock, $request, strlen($in));
	echo "OK.";


	/* Receives transport layer response and writes it to a file */
	echo "Waiting to Transport Layer to deliver response...";
	if (false === ($buf = socket_read($physicalSock, $MAX_BUF, PHP_NORMAL_READ))) {
        echo "socket_read() failed: reason: " . socket_strerror(socket_last_error($msgsock)) . "\n";
        break 2;
    }
    echo "Transport Layer Response received:";
    echo $buf;

    $file = fopen(RESPONSE_FILE, "w") or die("Unable to open response file!");
    $request = fwrite($file,$buf);
    fclose($file);

	echo "Notifying Physical layer...";
	socket_write($physicalSock, "OK.", strlen("OK."));

    socket_close($physicalSock);
    socket_close($transportSock);
    echo "My work is done here! *Flies away*"
} while (true);

?>