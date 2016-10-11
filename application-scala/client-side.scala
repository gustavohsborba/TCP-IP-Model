import java.net._
import java.io._
import scala.io._


val clientPort = 8080
val serverPort = 8081
val clientAddress = "127.0.0.1"
val serverAddress = "127.0.0.1"

val socketAplicacao = new ServerSocket(clientPort)

printf("Servidor escutando na porta %1$d", clientPort)

while (true) {
	//Recebendo da aplicacao
  val sAplicacao = socketAplicacao.accept()
  var outAplicacao = new DataOutputStream(sAplicacao.getOutputStream())
    
  //Estabelecendo conexao com o servidor
  val sServidor = new Socket(InetAddress.getByName(serverAddress), serverPort)
	val outServidor = new PrintStream(sServidor.getOutputStream())

	//Enviando requisicao ao servidor
	outServidor.println("GET /index.html HTTP/1.1")
	outServidor.flush()
	
  // Recebendo página:
	var inServidor = sServidor.getInputStream();
	var length = 1000
	var buffer = Array.fill[Byte](length)(0)
	var count = 0;
	while ({count = inServidor.read(buffer); count > 0}) {
    outAplicacao.write(buffer, 0, count);
  }
	
  // Fechando sockets e streams:
	inServidor.close()
	outServidor.close()
	outAplicacao.close()

	sServidor.close()
  sAplicacao.close()
}

socketAplicacao.close()