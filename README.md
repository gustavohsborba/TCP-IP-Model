# TCP-IP-Model

## About

CEFET-MG Computer Engineering's Computer Networks Systems class homework
proposed by Sandro Renato Dias <https://sites.google.com/site/sandrord>

Development group:
* [Gustavo Henrique de Souza Borba](https://github.com/gustavohsborba)  [gustavohsborba@gmail.com]
* [Bruno Marques Maciel]()  [bmarques.maciel@gmail.com]
* [Thiago Alexandre de Souza Silva](https://github.com/thiagoalexsilva)  [thiagoalexsilva93@gmail.com]
* [Ana Cláudia Gomes de Mendonça](https://github.com/gmanaclaudia)  [gmanaclaudia@gmail.com]


## Setting up Environment

Just run [install_dependencies.sh](https://github.com/gustavohsborba/TCP-IP-Model/blob/master/install_dependencies.sh)
and you'll have environment set up for all languages and frameworks used in
this project. You must run it as superuser, as it will install packages
and manage permissions into your system.

## Running layers:

### Physical Layer (C++)

To compile physical layer, just go to it's folder and run the following command:
```shell
 g++ -o physical-client client-side.c && g++ -o physical-server server-side.cpp
```
Now you have a client and a server executables. 

To start them, first execute the server:
```shell
 ./physical-server
```
Then execute the client with the following command:
```shell
 ./physical-client
```

Primeiro foi implementado um frame (da camada de enlace). Após o início da comunicação do socket, encontramos o MAC address do servidor e o cliente pergunta o tamanho do frame. Quando o servidor responde, o cliente começa a enviar um arquivo dividido em pacotes do tamanho certo para o servidor.


### Transport Layer (Python)

still in the making

### Network Layer (PHP)

still in the making

### Application Layer (Scala)

To execute this layer, run the command line below:
```shell
 scala client-side.scala
```
Para a execução desta, todos os arquivos envolvidos devem estar presentes na mesma pasta.

Primeiramente, o lado do cliente escreve a requisição de um arquivo com protocolo em HTTP em um arquivo .TXT. Este arquivo .TXT é passado atraves de uma linha de comando na qual há a chamada da parte cliente (camada física), juntamente com o endereço IP da parte servidor (camada física). No lado do servidor, o arquivo de requisição (agora em .SRV) é recebido, lido e suas informações interpretadas a fim de enviar o arquivo requerido ao cliente. Por fim, o cliente recebe o arquivo requerido e o imprime. Finalizando a execução.
