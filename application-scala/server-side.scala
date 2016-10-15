import java.net.ServerSocket
import java.net.InetAddress
import java.util.NoSuchElementException
import scala.io._
import java.io._


println("Received request...")
var requisitionFile = new File("request.srv")

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
fin.close()	

// Sending webpage to client:
var outToClient = new PrintWriter("response.srv", "UTF-8")
outToClient.println(statusLine)
outToClient.println(serverdetails)
outToClient.println(contentTypeLine)
outToClient.println(contentLengthLine)
val bufferedSource = Source.fromFile(fileName)
for (line <- bufferedSource.getLines) {
    outToClient.println(line)
}
bufferedSource.close()
outToClient.close()


println("Page sent:")
val bufferedSource = Source.fromFile("response.srv")
for (line <- bufferedSource.getLines) {
    println(line.toUpperCase)
}
bufferedSource.close()


println("\nRequest for " + fileName + " fulfilled");

