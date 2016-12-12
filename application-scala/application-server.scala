import java.net.ServerSocket
import java.net.InetAddress
import java.util.NoSuchElementException
import java.nio.file.Path;
import java.nio.file.Files;
import java.nio.file.FileSystems;
import scala.io._
import java.io._


val APPLICATION_PORT_SERVER = 41111;
val APPLICATION_PORT_CLIENT = 41112;


val servSock = new ServerSocket(APPLICATION_PORT_SERVER) 
// wait for transport request

while (true) {
    // Accept connection
    println("Scala Basic Web Server is Listening to port " + APPLICATION_PORT_SERVER +"...");
	val transportSock = servSock.accept()
    val transpSockRead = new BufferedSource(transportSock.getInputStream()).getLines()
    val transpSockWrite = new PrintStream(transportSock.getOutputStream())
    println("Connection accepted!");

    // Read request
	var request:String = ""
    if( transpSockRead.hasNext )
        request = transpSockRead.next()
    println("raw request: "+ request);
    
    // interpretting request:
    var query = request.split(" ")
    var method = query(0)
    var fileName = query(1)
    var protocolVersion = query(2)

    // Tries to open requested file. If fail, prepares to send a 404 page:
	var arq : File = new File("404.html")
	try {
		if(fileName.startsWith("/"))
			fileName = fileName.replace("/", "")
		if(fileName=="") 
			fileName = "index.html"
		arq = new File(fileName)
		if (!arq.isFile())
			throw new FileNotFoundException
		if(method.toUpperCase() == "POST")
			throw new NoSuchElementException
	} catch{
		case ex: FileNotFoundException => {
			fileName = "404.html"
			arq = new File(fileName);
		} 	
		case ex : NoSuchElementException => {
			fileName = "403.html"
			arq = new File(fileName);
		}
	}	

	// Builds the message:
	var fin = new FileInputStream(fileName)
	var statusLine = protocolVersion + " 200 OK" + "\r\n";
	var serverdetails = "Server: Scala HTTPServer";
	var contentLengthLine = "Content-Length: " + Integer.toString(fin.available()) + "\r\n";
	var contentTypeLine = "Content-Type: " + "text/html" + "\r\n";
	fin.close()	

    // sends webpage trough connection
    println("Sending response...")
	transpSockWrite.println(statusLine)
	transpSockWrite.println(serverdetails)
	transpSockWrite.println(contentTypeLine)
	transpSockWrite.println(contentLengthLine)
    var bufferedSource = Source.fromFile(fileName)
	for (line <- bufferedSource.getLines) {
	    transpSockWrite.println(line)
	}
    transpSockWrite.flush();
    println("Response sent!")
    
    transportSock.close
}

servSock.close












// Sending webpage to client:
/*
var outToClient = new PrintWriter("response.srv", "UTF-8")
outToClient.println(statusLine)
outToClient.println(serverdetails)
outToClient.println(contentTypeLine)
outToClient.println(contentLengthLine)
var bufferedSource = Source.fromFile(fileName)
for (line <- bufferedSource.getLines) {
    outToClient.println(line)
}
bufferedSource.close()
outToClient.close()


println("Page sent:")
bufferedSource = Source.fromFile("response.srv")
for (line <- bufferedSource.getLines) {
    println(line.toUpperCase)
}
bufferedSource.close()


println("\nRequest for " + fileName + " fulfilled");

*/

