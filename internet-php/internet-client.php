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
$PHYSICAL_PORT_CLIENT = 11112;
$INTERNET_PORT_CLIENT = 21112;
$MAX_BUF = 81920;




$listener = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
socket_bind($listener, $LOCALHOST_IP, $INTERNET_PORT_CLIENT);
socket_listen($listener, 5);

do {
	echo "Internet Layer listening to port $INTERNET_PORT_CLIENT...\n";
    $transportSock = socket_accept($listener);

    echo "transport layer connection stabilished!\n";
    $buf = socket_read($transportSock, $MAX_BUF);
    
    
    /* *************************************************************
        DO SOME INERNET LAYER PROCESSING HERE
    ************************************************************** */
    $buf = preg_replace('~[\r\n]+~', '', $buf);
    $exploded = explode(" ", $buf);
    $hostname = "";
    foreach ($exploded as $field) {
        if(strpos($field, 'Host') !== false){
            $hostArray = explode(":", $field);
            $hostname = $hostArray[1];
        }
    }
    $ipdest = gethostbyname( $hostname );
    $iplocal = getHostByName(getHostName());
    
    
    /* *************************************************************
        INERNET LAYER PROCESSING: adds IP data on package and delivers.
    ************************************************************** */

    $request = "ipdest:$ipdest|iporig:$iplocal|$buf";
    echo "MESSAGE:\n\t$request\n";


	/* Connecting with physical layer */
	echo "Connecting with physical layer...  ";
	$physicalSock = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
	$result = socket_connect($physicalSock, $LOCALHOST, $PHYSICAL_PORT_CLIENT);
	echo "OK.\n";
	
    /* Sending package to physical layer and waiting response */
    echo "Sending IP package to physical layer...\n";
	$bsent = socket_write($physicalSock, $request);
    echo "$bsent bytes sent\n";

	/* Receives physical layer response */
	echo "Waiting to physical layer to deliver response...\n";
	$buf = socket_read($physicalSock, $MAX_BUF);
    echo "physical layer Response received! \n";
    echo "MESSAGE:\n\t$buf\n";

    
    /* *************************************************************
        INERNET LAYER PROCESSING: removes IP data of package and delivers.
    ************************************************************** */

    // Remove IP Packing
    $exploded = explode("|", $buf);
    $response = $exploded[2];


    // send package to transport layer
	echo "\nSending package to transport layer...\n";
	$bsent = socket_write($transportSock, $response);
    echo "$bsent bytes sent\n";

    // closing physical socket
    $bsent = socket_write($physicalSock, "Thank you! Closing connection...");
    socket_close($physicalSock);

    // closing transport socket.
    socket_close($transportSock);
    echo "My work is done here! *Flies away*\n\n\n";
} while (true);

?>