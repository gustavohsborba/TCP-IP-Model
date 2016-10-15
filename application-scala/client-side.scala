import java.net._
import java.io._
import scala.io._
import scala.sys.process._


val serverPort = 8081
val clientPort = 63050
val networkPort = 63051
var clientAddress = "127.0.0.1"
var serverAddress = "127.0.0.1"
var page = "/index.html"

if(args.length == 1)
	serverAddress = args(0)


// Escrevendo a requisição para a camada física utilizar:
var writer = new PrintWriter("request.txt", "UTF-8")
println("Sending request: ")
println("GET "+ page + " HTTP/1.1")
writer.println("GET "+ page + " HTTP/1.1")
writer.close()

// Enviando para a camada física:
val command = "./physical-client "+serverAddress + " request.txt"
Process(command)!
new File("request.txt").delete()

// Recebendo e imprimindo página:
println("Response received from server:")
val bufferedSource = Source.fromFile("response.txt")
for (line <- bufferedSource.getLines) {
    println(line.toUpperCase)
}
bufferedSource.close()

new File("response.txt").delete()
