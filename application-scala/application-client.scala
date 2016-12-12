import java.net._
import java.io._
import scala.io._
import scala.sys.process._
import java.nio.file.Path;
import java.nio.file.Files;
import java.nio.file.FileSystems;


val INTERNET_PORT_SERVER = 21111
val INTERNET_PORT_CLIENT = 21112
val TRANSPORT_PORT_SERVER = 31111
val TRANSPORT_PORT_CLIENT = INTERNET_PORT_CLIENT//31112
val APPLICATION_PORT_SERVER = 41111
val APPLICATION_PORT_CLIENT = 41112

var localhostAddress = "127.0.0.1"
var serverAddress = "127.0.0.1"
var page = "/index.html"
var requestedPage = serverAddress + page


if(args.length >= 1)
	serverAddress = args(0)
if(args.length >= 2)
	page = args(1)
val request = "GET " + page + " HTTP/1.1 Host:" + serverAddress
println("Application started! Request is:" + request)


val transportSock = new Socket(localhostAddress,TRANSPORT_PORT_CLIENT)
val writetransportSock = new PrintStream(transportSock.getOutputStream())
val transportSockRead = new BufferedSource(transportSock.getInputStream()).getLines()
println("Connected to Transport layer on port " + TRANSPORT_PORT_CLIENT + ". Sending request...")


writetransportSock.println(request)
writetransportSock.flush()
println("Application waiting for response...")

//wait for layer response
var response:String = "";
if(transportSockRead.hasNext)
    response = transportSockRead.next()
println("Application Received: " + response)
 

// Receiving and printing page:
println("Response received from server:")
val bufferedSource = Source.fromFile("response.txt")
for (line <- bufferedSource.getLines) {
    println(line.toUpperCase)
}
bufferedSource.close()

var path = FileSystems.getDefault().getPath("request.txt")
var deu = Files.deleteIfExists(path)
path = FileSystems.getDefault().getPath("response.txt")
deu = Files.deleteIfExists(path)


