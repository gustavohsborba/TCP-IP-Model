import java.io.File
import java.io.FileInputStream
import java.io.DataOutputStream
import java.net.ServerSocket
import java.net.InetAddress
import scala.io._
 
    
var port = 8081
var listener = new ServerSocket(port, 100, InetAddress.getByName("127.0.0.1")); // Backlog: 100 - The maximum queue length for incoming connection indications (a request to connect) is set to the backlog parameter.
printf("Servidor escutando na porta %1$d", port)
 
while (true) {
    var sock = listener.accept()
    
    //Recebendo solicitacao 
    val in = new BufferedSource(sock.getInputStream()).getLines()
    
    if (in.next() == "Send") {   
    
    	var fileName = "testando.html"
    
    	var arq = new File(fileName)
    
    	if (arq.isFile()){
    		var statusLine = "HTTP/1.1 200 OK" + "\r\n"
    		var serverdetails = "Server: Java HTTPServer"
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
		}
		
		
		//println("Enviado do servidor")
		
	
	}
    
    sock.close()
}

listener.close()
