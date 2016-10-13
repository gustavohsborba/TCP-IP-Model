import java.io.File
import java.io.FileInputStream
import java.io.DataOutputStream
import java.net.ServerSocket
import java.net.InetAddress
import java.io.FileNotFoundException
import java.util.NoSuchElementException
import scala.io._


def error( message : String){ 
	println(message)
	System.exit(0);
}

var serverPort = 63040
var networkPort = 63041 // Network layer port
var port = 8081 // Application port



// Backlog: 100 - The maximum queue length for incoming connection 
// indications (a request to connect) is set to the backlog parameter.
var listener = new ServerSocket(port, 100, InetAddress.getByName("127.0.0.1")); 
printf("\nServidor escutando na porta %1$d", port)
 



while (true) {
	var sock = listener.accept()

	try {
	
	/* PARA PROBLEMA AO ATUALIZAR
		var lastReqFile = new File("last.txt")
		if (requisitionFile == null) {
			var writer = new PrintWriter("requisition.txt", "UTF-8")
    		writer.println("1")
    		writer.close()
    	}
    	else {
    		val inAux = new BufferedSource(new FileInputStream(lastReqFile)).getLines()
			val firstLineAux = inAux.next().split("\\s+")
			var requisitionFile = new File("requisition.txt")
			// e dai fazer verificacao de numeros e tirar o delete
    	}
    	
    	*/
	
		var requisitionFile = new File("requisition.txt")
		/*while (requisitionFile == null)
			requisitionFile = new File("requisition.txt")*/
		/*
		var command = "sudo ./clientExeC localhost requisition.txt"
		var p  = Runtime.getRuntime().exec(command)
		p.waitFor()
		*/	
	
		//Receiving request
		// We are interested just in the first line of the protocol
		//val in = new BufferedSource(sock.getInputStream()).getLines()
		
		val in = new BufferedSource(new FileInputStream(requisitionFile)).getLines()
		val firstLine = in.next().split("\\s+"); 
		val method = firstLine(0); // GET or POST
		var fileName = firstLine(1).substring(1); // substring removes the first "/" character
		val protocolVersion = firstLine(2); // HTTP/1.1
		//while(in.hasNext) printf("\n"+in.next());
		
		requisitionFile.delete()


		// Tries to open requested file. If fail, prepares to send a 404 page:
		var arq : File = new File("404.html");
		try {
			if(fileName=="") 
				fileName = "index.html";
			arq = new File(fileName)
			if (!arq.isFile())
				throw new FileNotFoundException;
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
		var fin = new FileInputStream(fileName)


		// Builds the message:
		var statusLine = protocolVersion + " 200 OK" + "\r\n";
		var serverdetails = "Server: Scala HTTPServer";
		var contentLengthLine = "Content-Length: " + Integer.toString(fin.available()) + "\r\n";
		var contentTypeLine = "Content-Type: " + "text/html" + "\r\n";
		//((fileName.endsWith(".htm") || fileName.endsWith(".html"))? "text/html" : "") + 


		// Sending webpage to client:
		var outToClient = new DataOutputStream(sock.getOutputStream())
		outToClient.writeBytes(statusLine)
		outToClient.writeBytes(serverdetails)
		outToClient.writeBytes(contentTypeLine)
		outToClient.writeBytes(contentLengthLine)
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

		println("\nRequest for " + fileName + " fulfilled");

	} catch {
		case ex : NoSuchElementException => {}
	}
}

listener.close()