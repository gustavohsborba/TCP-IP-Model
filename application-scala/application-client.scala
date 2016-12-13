import java.net._
import java.io._
import scala.io._
import java.lang._
import java.util._
import scala.sys.process._
import java.nio.file.Path;
import java.nio.file.Files;
import java.nio.file.FileSystems;


val PHYSICAL_PORT_SERVER = 11111
val PHYSICAL_PORT_CLIENT = 11112
val INTERNET_PORT_SERVER = 21111
val INTERNET_PORT_CLIENT = 21112
val TRANSPORT_PORT_SERVER = 31111
val TRANSPORT_PORT_CLIENT = 31112//PHYSICAL_PORT_CLIENT//
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
Thread sleep 5000

val transportSock = new Socket(localhostAddress,TRANSPORT_PORT_CLIENT)
val writetransportSock = new PrintStream(transportSock.getOutputStream())
println("Connected to Transport layer on port " + TRANSPORT_PORT_CLIENT + ". Sending request...")
writetransportSock.print(request)
writetransportSock.flush()


//wait for layer response
println("Application waiting for response...")
var response:String = "";
val transportSockRead = new Scanner(transportSock.getInputStream())

println("GOT RESPONSE:")

while (transportSockRead.hasNextLine()) {
    val line = transportSockRead.nextLine()
    println(line)
}


writetransportSock.println("Thank you! I'm closing this connection!")
transportSock.close()
//println("Application Received: " + response)
 

// Receiving and printing page:
//println("Response received from server:")
//val bufferedSource = Source.fromFile("response.txt")
//for (line <- bufferedSource.getLines) {
//    println(line.toUpperCase)
//}
//bufferedSource.close()

//var path = FileSystems.getDefault().getPath("request.txt")
//var deu = Files.deleteIfExists(path)
//path = FileSystems.getDefault().getPath("response.txt")
//deu = Files.deleteIfExists(path)


