import java.io.File
import java.io.FileInputStream
import java.io.DataOutputStream
import java.net.ServerSocket
import java.net.InetAddress
import scala.io._


def error( message : String){ 
	println(message)
	System.exit(0);
}

 
    
var port = 8081

// Backlog: 100 - The maximum queue length for incoming connection 
// indications (a request to connect) is set to the backlog parameter.
var listener = new ServerSocket(port, 100, InetAddress.getByName("127.0.0.1")); 
printf("\nServidor escutando na porta %1$d", port)
 

while (true) {
    var sock = listener.accept()
    
    //Receiving request
    val in = new BufferedSource(sock.getInputStream()).getLines()
    val firstLine = in.next().split("\\s+"); 
    val method = firstLine(0); // GET or POST
    val fileName = firstLine(1).substring(1); // substring removes the first "/" character
    val protocolVersion = firstLine(2); // HTTP/1.1

    // We are interested just in the first line.
	//while(in.hasNext) printf("\n"+in.next());
    //if ( !(in.next() == "Send"))  
    	//error("this is not the client we're looking for!");
    
	var arq = new File(fileName)

	if (!arq.isFile())
		error("\n404: Internal Server Error - File not Found");
	
	var statusLine = method + " 200 OK" + "\r\n"
	var serverdetails = "Server: Scala HTTPServer"
	//var contentLengthLine = "Content-Length: " + Integer.toString(fin.available()) + "\r\n"
	var contentTypeLine = "Content-Type: text/html" + "\r\n"


  	var fin = new FileInputStream(fileName)
    //if (!fileName.endsWith(".htm") && !fileName.endsWith(".html")) //DA PRA TIRAR ESSA PARTE?
	  	//contentTypeLine = "Content-Type: \r\n"

    var outToClient = new DataOutputStream(sock.getOutputStream())
	
	outToClient.writeBytes(statusLine)
	outToClient.writeBytes(serverdetails)
	outToClient.writeBytes(contentTypeLine)
	//outToClient.writeBytes(contentLengthLine)
	outToClient.writeBytes("Content-Length: " + Integer.toString(fin.available()) + "\r\n")
	outToClient.writeBytes("Connection: close\r\n")
	outToClient.writeBytes("\r\n")
	
	/*var length = arq.length.toInt
	var buffer = Array.fill[Byte](length)(0)
	outToClient.write(buffer, 0, buffer.length)*/

	var length = 1000
	var buffer = Array.fill[Byte](length)(0)
	var bytesRead = 0
	while ({bytesRead = fin.read(buffer); bytesRead != -1}) {
		outToClient.write(buffer, 0, bytesRead)
	}

	fin.close()	
	outToClient.close()
    sock.close()

    //println("Enviado do servidor")
}

listener.close()
