
/*
 * Simulates a physical layer from the TCP/IP model.
 * Following this tutorial: http://www.linuxhowtos.org/C_C++/socket.htm
 * possibly, the following too: http://www.dicas-l.com.br/arquivo/programando_socket_em_c++_sem_segredo.php#.V9lODHUrLDc
 */




/* 
  Camada Física:
  
  
  Recebe arquivo
  Transforma em binário
  
  Conectar socket
  Recebe a quantidade de bits
  Envia em pacotes da sugerida quantidade



*/


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/sendfile.h>
#include <netinet/in.h>
#include <netdb.h> 

#include <pthread.h>
#include <time.h>


#define MIN_MSG_BUFF 256
#define MIN_MSG_SIZE 255
#define PORT_NUMBER 8766
//#define MAX_BUF    8191
#define BUF_SIZ     8192



// Para poder fazer if's com uma linha só:
void error(const char *msg){
    perror(msg);
    exit(1);
}





int main(int argc, char *argv[])
{
	int MAX_BUF;
    int sockfd,  n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char buffer[MIN_MSG_BUFF];
    if (argc < 2) {
       fprintf(stderr,"usage %s IP_or_hostname filename\n", argv[0]);
       exit(0);
    }
    
    // Opening socket to start connection:
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");
    // resolving host:
    server = gethostbyname(argv[1]);
    if (server == NULL) 
        error("ERROR, no such host\n");
    bzero((char *) &serv_addr, sizeof(serv_addr)); // cleans serv_addr
    
    // configuring and connecting socket:
    serv_addr.sin_family = AF_INET;
    bcopy( (char *)server->h_addr,  
    	   (char *)&serv_addr.sin_addr.s_addr,   
    	   server->h_length);
    serv_addr.sin_port = htons(PORT_NUMBER);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");

    // Connection stabilished. Sending message requesting frame size:
    bzero(buffer,MIN_MSG_BUFF);
    strcpy(buffer,"Yo, bro! What's the Frame size?"); 
    n = write(sockfd,buffer,strlen(buffer));
    if (n < 0) 
    	error("ERROR writing to socket");
    
    // Reading message from server: 
    bzero(buffer,MIN_MSG_BUFF);
    n = read(sockfd,buffer,MIN_MSG_SIZE);
    if (n < 0) 
         error("ERROR reading from socket");
    printf("Message size: %s\n",buffer);
    MAX_BUF = strtol (buffer,NULL,10);
    
    // Sending filename to server:
    bzero(buffer,MIN_MSG_BUFF);
    strcpy(buffer,argv[2]); 
    n = write(sockfd,buffer,strlen(argv[2]));
    //bzero(buffer,MIN_MSG_BUFF);
    //n = read(sockfd,buffer,MIN_MSG_SIZE);
    //printf("Return from server: %s\n", buffer);
    //if(n < 9)
    //    error("Server is not OK!");
    
    
    // Opening message file:
    FILE* msgFile;
    msgFile = fopen(argv[2],"rb");
    int msgFd = fileno(msgFile);
    if( !msgFile | msgFd < 0)
    	error("File doesn't exist");
	
	// Sending messages of the size of defined frame:
	long offset = 0;
	while(sendfile(msgFd,sockfd,&offset,MAX_BUF) > 0){
	    offset += MAX_BUF;
		printf("offset: %ld\n", offset);
	}
	
	printf("File sent.\n");
    //n = write(sockfd,msgFile,fileLength);
    
    close(sockfd);
    return 0;
}












