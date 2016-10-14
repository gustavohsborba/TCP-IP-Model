/*
 * Simulates a physical layer from the TCP/IP model.
 *
 * A simple server in the internet domain using TCP
 * The port number is passed as an argument
 *
 */


/*  */

#include "physical.h"

        char this_mac[MAC_SIZE], cli_mac[MAC_SIZE];



void receiveFile(int sockfd,char *filename){
    // Initiating file transfer. Firstly, opening file to write:
    FILE* file;
    int fd;
    file = fopen(filename,"wb");
    fd = fileno(file);
    if( !file | fd < 0)
        error("Couldn't create file!");

    // Actually receiving and writing file:
    struct Frame frame = {};
    char buffer[BUF_SIZ];
    while(1) {
        bzero(buffer, BUF_SIZ);
        receiveFrame(&frame, sockfd);
        getData(&frame, buffer);
        size_t len = strlen(buffer);
        printf("Message of %d bytes received from client\n\n", (int) len);
        if ((int) len <= 0) break;
        else fwrite(buffer, sizeof(char), len, file);
    }
    printf("File received.\n");
    fclose(file);
}



void sendFile(int sockfd,char *filename){
    FILE* msgFile;
    msgFile = fopen(filename,"rb");
    int msgFd = fileno(msgFile);
    if( !msgFile | msgFd < 0)
        error("File doesn't exist");


    // Finally, Sending files:
    int i = 0;
    int c;
    char buffer[BUF_SIZ];
    struct Frame frame = {};
    bzero(buffer,BUF_SIZ);
    size_t nbytes = fread(buffer, sizeof(char), MAX_DATA_SIZE-1, msgFile);
    while (nbytes > 0){
        createFrame(&frame, buffer, this_mac, cli_mac);
        printf("\nsending message of %d bytes to server...\n", strlen(frame.data));
        sendFrame(&frame, sockfd, frameSize(&frame));
        bzero(buffer,BUF_SIZ);
        nbytes = fread(buffer, sizeof(char), MAX_DATA_SIZE, msgFile);
    }
    bzero(buffer,BUF_SIZ);
    createFrame(&frame, buffer, this_mac, cli_mac);
    sendFrame(&frame, sockfd, frameSize(&frame));
    printf("file sent.\n");
    fclose(msgFile);
}



int main(int argc, char *argv[])
{
    int listener, sockfd; // Socket file descriptors
    struct sockaddr_in serv_addr, cli_addr, network_addr;
    socklen_t clilen, network_layer;

    // Opening socket to listen to connection:
    listener = socket(AF_INET, SOCK_STREAM, 0);
    if (listener < 0)
        error("ERROR opening socket");

    // preparing socket:
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(PORT_NUMBER);
    if (bind(listener, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR on binding");
    listen(listener,5);

    while(1){

        char buffer[BUF_SIZ], filename[BUF_SIZ];
        struct Frame frame = {};

        // Accepting Connection. Receiving message request for frame size:
        sockfd = accept(listener, (struct sockaddr *) &cli_addr,  &clilen);
        if (sockfd < 0)
            error("ERROR on accept");

        // Cheating to get client's MAC addres.
        // THE CORRECT WAY IS USING ARP COMMAND.
        char mac[10];
        bzero(mac,10);
        receiveMessage(sockfd, mac);
        strncpy(cli_mac, mac, 6);
        getMAC(mac);
        bytesToStr(mac, buffer, 6);
        mac[6] = '\0';
        sendMessage(sockfd, mac);
        strncpy(this_mac, mac, 6);



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
        getMAC(this_mac);
        getMAC(cli_mac);
        strcpy(buffer,"Ready to Transfer Files...");
        createFrame(&frame, buffer, this_mac, cli_mac);
        sendFrame(&frame, sockfd, frameSize(&frame));

        
        // Receiving request:
        receiveFile(sockfd, (char*) "request.srv");

        // calls upper layers:
        printf("\n\ncalling system\n");
        system("scala server-side.scala");

        // After upper layers complete processing, get response:
        sendFile(sockfd, "response.srv");


        close(sockfd);
        // return to loop and wait for next connection
    }
    return 0;
}
