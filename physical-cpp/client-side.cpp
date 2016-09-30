/*
 * Simulates a physical layer from the TCP/IP model.
 * Following this tutorial: http://www.linuxhowtos.org/C_C++/socket.htm
 * possibly, the following too: http://www.dicas-l.com.br/arquivo/programando_socket_em_c++_sem_segredo.php#.V9lODHUrLDc
 */

//#include<bits/stdc++.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <pthread.h>
#include <time.h>


/* sockaddr_in is defined in netinet/in.h:
 * struct sockaddr_in {
 *     short   sin_family; // must be AF_INET
 *     u_short sin_port;
 *     struct  in_addr sin_addr;
 *     char    sin_zero[8]; // Not used, must be zero
 * };
 */

 /*class Frame{
    	unsigned char sourceMAC[6]; // source's MAC address (6 octets)
    	unsigned char destinationMAC[6]; // destination's MAC address (6 octets)
    	unsigned char protocol[2]; // type of protocol - 0x0800 for IPv4
    	unsigned char data[1002]; // 1006 bytes limit for data
    	unsigned char checksum[8]; // to check for transmission errors
    public:
        Frame(char src_mac[], char dst_mac[], char message[]) {
        	strcpy(sourceMAC, src_mac);
        	strcpy(destinationMAC, dst_mac);
        	strcpy(protocol, "\x08\x00");
        	sprintf(data, "%-494s", message);
        	strcpy(checksum, "\x00\x20\x20\x3a");
        }
        ~Frame()
};*/



#define PORT_F		65111
#define PORT_REQ	65002
#define MAX_BUF		8192



// Para poder fazer if's com uma linha só:
void error(char *msg){
    perror(msg);
    exit(1);
}



int main(int argc, char * argv[]){
    
    struct sockaddr_in  readSocket, localSocket; // sockets to connect
    int localSocketfd, readSocketfd; // sockets' file descriptors
    
    int portno, n, bindStatus;
    char buffer[MAX_BUF], raw[MAX_BUF],rawrequest[MAX_BUF],*temp,*response;
    char *ip, n[20];
    FILE *arq;


    // Initializing socket to stabilish connection:
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");
    bzero(&localSocket,sizeof(localSocket)); // Fill socket with zeroes
    localSocket.sin_family=AF_INET;
	localSocket.sin_addr.s_addr=INADDR_ANY;
	localSocket.sin_port=htons(PORT_REQ);

	// Binding socket to it's address.
	bindStatus = bind(localSocketfd, (struct sockaddr *) &localSocket, sizeof(localSocket))
	if( bindStatus < 0){
	    if(localSocketfd >= 0)
	        close(localSocketfd);
	    error("Error binding local socket.");
	}


	while(1){

	    // waiting socket to receive frame:
        printf("Listening...\n");
		listen(localSocketfd,5);
		readSocketfd=accept(localSocketfd, (struct sockaddr *) &readSocket, &sizeof(readSocket));
		if(readSocketfd < 0)
			error("Error receiving local socket");
		printf("socket received!\n");

        bzero(&buffer,sizeof(buffer)); // cleaning buffer before reading
        bread = recv(readSocketfd,buffer,MAX_BUF,0) <= 0; 

        printf("request: %s\n",buffer);
		strcpy(rawrequest,buffer);


	}
	printf("\nConnection closed.\n\n");


/* 
  Camada Física:
  
  
  Recebe arquivo
  Transforma em binário
  
  Conectar socket
  Recebe a quantidade de bits
  Envia em pacotes da sugerida quantidade



*/



    // A partir daqui, não sabemos o que estamos fazendo...


    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serverSocket, sizeof(serverSocket));
    serverSocket.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
         (char *)&serverSocket.sin_addr.s_addr,
         server->h_length);
    serverSocket.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serverSocket,sizeof(serverSocket)) < 0)
        error("ERROR connecting");
    printf("Please enter the message: ");
    bzero(buffer,256);
    fgets(buffer,255,stdin);
    n = write(sockfd,buffer,strlen(buffer));
    if (n < 0)
         error("ERROR writing to socket");
    bzero(buffer,256);



}

using namespace std;