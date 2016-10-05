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




struct Frame{
    char preamble[8]; // 8 bytes for preamble
    char sourceMAC[6]; // source's MAC address
    char destinationMAC[6]; // destination's MAC address
    char ethernetType[2];// type of protocol - 0x0800 for Ethernet in IPv4
    char data[494]; // 494 bytes limit for data
    char checksum[4];
};


// Para poder fazer if's com uma linha só:
void error(const char *msg){
    perror(msg);
    exit(1);
}


void createFrame(struct Frame *frame, char *message, char* src_mac, char* dst_mac){
    strcpy (frame->preamble, "\x03\x21\x41\x3f\x04\x21\x41\x3f"); // pre-defined preamble.
    strcpy (frame->sourceMAC, src_mac);
    strcpy (frame->destinationMAC, dst_mac);
    strcpy (frame->ethernetType, "\x08\x00"); // type of protocol - 0x0800 for IPv4
    sprintf(frame->data, "%s", message);
    strcpy (frame->checksum, "\x30\x21\x00\x00"); // no checksum calculation. Ain't nobody got time fo dat
}

void sendFrame(struct Frame *frame, int socket){
    int sent_bytes;
    sent_bytes = send(socket, frame, sizeof(frame), 0);
    fprintf(stdout, "Client sent %d bytes to the server\n", (int) sent_bytes);
    if (sent_bytes < 0)
        error("ERROR writing to socket");
}


void printFrame(struct Frame *frame){
    printf("Frame:\n\tpreamble: %s\n",frame->preamble);
    printf("\tsourceMAC: %s\n",frame->sourceMAC);
    printf("\tdestinationMAC: %s\n",frame->destinationMAC);
    printf("\tethernetType: %s\n",frame->ethernetType);
    printf("\tdata: %s\n",frame->data);
    printf("\tchecksum: %s\n",frame->checksum);
}

void receiveFrame(struct Frame *frame, int sockfd){
    char buffer[MAX_BUF];
    bzero(buffer,BUF_SIZ);
    int n = read(sockfd,buffer,MAX_BUF);
    if (n<0)
        error("Could not read from socket");
    printf("Buffer: %s (%d bytes)\n", buffer, n);

    strncpy (frame->preamble, buffer, 8);
    strncpy (frame->sourceMAC, buffer+8, 6);
    strncpy (frame->destinationMAC, buffer+14, 6);
    strncpy (frame->ethernetType, buffer+20, 2);
    strncpy (frame->data, buffer+22, 4);
    strncpy (frame->checksum, buffer, 4);
    printf("SUCESSO!!! data: %s \n", frame->data);
    printFrame(frame);
}

int main(int argc, char *argv[])
{
	
    int sockfd, newsockfd;
    socklen_t clilen;
    char buffer[BUF_SIZ], filename[BUF_SIZ];
    struct sockaddr_in serv_addr, cli_addr;
    int n;
    struct Frame frame = {};
    
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
    
    // Sending frame size response:
    bzero(buffer, BUF_SIZ);
    sprintf(buffer,"%d",MAX_BUF);
    n = write(newsockfd,buffer, strlen(buffer));
    if (n < 0) 
    	error("ERROR writing to socket");
    	
    // Receiving filename and sending OK message:
    bzero(filename,BUF_SIZ);
    n = read(newsockfd,filename,MAX_BUF);
    //receiveFrame(&frame, sockfd);
    //strcpy(filename, frame.data);
    strcat(filename, "2");
    printf("File Name should be %s\n", filename);
    bzero(buffer, BUF_SIZ);
    sprintf(buffer,"SUCCESS!!!");
    n = write(newsockfd,buffer, strlen(buffer));
    if (n < 0)
     	error("ERROR writing to socket");
    printf("%d bytes sent to client\n", n);
    
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