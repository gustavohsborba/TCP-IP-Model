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

#include "physical.h"




int waitForClient(){
    struct sockaddr_in serv_addr, cli_addr;
    int sockfd, newsockfd;
    socklen_t clilen;

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

    // Accepting Connection. Receiving message request for frame size:
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr,  &clilen);
    if (newsockfd < 0)
        error("ERROR on accept");
    return newsockfd;
}




int main(int argc, char *argv[])
{

    int  sockfd;
    char buffer[BUF_SIZ], filename[BUF_SIZ];
    char *this_mac, *cli_mac;
    struct Frame frame = {};

    sockfd = waitForClient();


    bzero(buffer,BUF_SIZ);
    receiveMessage(sockfd, buffer);

    // Sending frame size response:
    bzero(buffer, BUF_SIZ);
    sprintf(buffer,"%d",MAX_BUF);
    sendMessage(sockfd, buffer);

    // Receiving filename:
    bzero(filename,BUF_SIZ);
    receiveFrame(&frame, sockfd);
    strcpy(filename, frame.data);
    strcat(filename, "2");
    printf("File Name should be %s\n", filename);

    // Sending OK message
    this_mac = "abcdef";
    cli_mac = "abcdef";
    strcpy(buffer,"Ready to Transfer Files...");
    createFrame(&frame, buffer, this_mac, cli_mac);
    sendFrame(&frame, sockfd, frameSize(&frame));

    // Initiating file transfer. Firstly, opening file to write:
    FILE* file;
    int fd;
    file = fopen(filename,"wb");
    fd = fileno(file);
    if( !file | fd < 0)
    	error("Couldn't create file!");

    // Actually receiving and writing file:
    while(1) {
        bzero(buffer, MAX_BUF);
        receiveFrame(&frame, sockfd);
        size_t len = strlen(frame.data);
        strcpy(buffer, frame.data);
        //int len = read(sockfd, buffer, MAX_BUF);
        printf("%d bytes received from client\n", (int) len);
        if ((int) len <= 0) break;
        else fwrite(buffer, sizeof(char), len, file);
    }


    printf("File received.\n");

    fclose(file);
    close(sockfd);
    return 0;
}