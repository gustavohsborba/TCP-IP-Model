/*
 * Simulates a physical layer from the TCP/IP model.
 *
 * A simple server in the internet domain using TCP
 * The port number is passed as an argument
 *
 */


/*  */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/sendfile.h>
#include <netinet/in.h>



#define PORT_NUMBER 8766
#define MAX_BUF	    8191
#define BUF_SIZ     8192



void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
	
    int sockfd, newsockfd;
    socklen_t clilen;
    char buffer[BUF_SIZ], filename[BUF_SIZ];
    struct sockaddr_in serv_addr, cli_addr;
    int n;
    
    // Opening socket to listen to connection:
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
       error("ERROR opening socket");

	// preparing socket:
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(PORT_NUMBER);
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR on binding");
    listen(sockfd,5);
    clilen = sizeof(cli_addr);
    
    // Accepting Connection. Receiving message request for frame size:
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr,  &clilen);
    if (newsockfd < 0)
        error("ERROR on accept");
    bzero(buffer,BUF_SIZ);
    n = read(newsockfd,buffer,MAX_BUF);
    if (n < 0) 
    	error("ERROR reading from socket");
    printf("Received message: %s\n",buffer);
    
    // Sending frame size request:
    bzero(buffer, BUF_SIZ);
    sprintf(buffer,"%d",MAX_BUF);
    n = write(newsockfd,buffer, sizeof(buffer));
    if (n < 0) 
    	error("ERROR writing to socket");
    	
    // Receiving filename and sending OK message:
    bzero(filename,BUF_SIZ);
    n = read(newsockfd,filename,MAX_BUF);	
    strcat(filename, "2");
    printf("File Name should be %s\n", filename);
    //bzero(buffer, BUF_SIZ);
    //n = write(newsockfd,"SUCCESS!!!", 10);
    //if (n < 0) 
    // 	error("ERROR writing to socket");
    
    FILE* file;
    int fd;
    file = fopen(filename,"wb");
    fd = fileno(file);
    if( !file | fd < 0)
    	error("Couldn't create file!");	
    
    long offset = 0;
	while(sendfile(sockfd,fd,&offset,(long)MAX_BUF) > 0) {
	    offset += MAX_BUF;
	}
    printf("File received.\n");
    
    close(newsockfd);
    close(sockfd);
    return 0;
}
