import java.net._
import java.io._
import scala.io._
import scala.sys.process._
import java.nio.file.Path;
import java.nio.file.Files;
import java.nio.file.FileSystems;


val serverPort = 8081
val clientPort = 63050
val networkPort = 63051
var clientAddress = "127.0.0.1"
var serverAddress = "127.0.0.1"
var page = "/index.html"

if(args.length >= 1)
	serverAddress = args(0)
if(args.length >= 2)
	page = args(1)

// Escrevendo a requisição para a camada física utilizar:
var writer = new PrintWriter("request.txt", "UTF-8")
println("Sending request: ")
println("GET "+ page + " HTTP/1.1")
writer.println("GET "+ page + " HTTP/1.1")
writer.close()

// Enviando para a camada física:
val command = "./physical-client "+serverAddress + " request.txt"
Process(command)!

// Recebendo e imprimindo página:
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
