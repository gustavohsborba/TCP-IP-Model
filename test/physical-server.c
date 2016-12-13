/*
 * Simulates a physical layer from the TCP/IP model.
 *
 * A simple server in the internet domain using TCP
 * The port number is passed as an argument
 *
 */


#include "physical.h"





int main(int argc, char *argv[])
{
    int listener, // Socket file descriptors
        sockfd,   // a listener, a communication socket,
        sockfdil; // and a internet layer socket
    struct sockaddr_in client_addr;
    socklen_t client_len = 0;
    char this_mac[MAC_SIZE], cli_mac[MAC_SIZE];

    printf("\n");
    printf("Server's Physical layer listening to port %d...\n", PORT_NUMBER);
    listener = startListening(PORT_NUMBER);

    while(1){

        // Accepting Connection. Receiving message request for frame size:
        sockfd = accept(listener, (struct sockaddr *) &client_addr,  &client_len);
        if (sockfd < 0)
            error("ERROR on accept");
        printf("\nConnection stabilished!");

        char buffer[BUF_SIZ], filename[BUF_SIZ];
        struct Frame frame = {};

        // Cheating to get client's MAC addres.
        // THE CORRECT WAY IS USING ARP COMMAND.
        bzero(buffer,10);
        receiveMessage(sockfd, buffer);
        strncpy(cli_mac, buffer, 6);
        getMAC(buffer);
        buffer[6] = '\0';
        sendMessage(sockfd, buffer);
        strncpy(this_mac, buffer, 6);


        bzero(buffer,BUF_SIZ);
        receiveMessage(sockfd, buffer);

        // Sending frame size response: 
        printf("\nNegotiating frame size...");
        bzero(buffer, BUF_SIZ);
        sprintf(buffer,"%d",MAX_BUF);
        sendMessage(sockfd, buffer);
        
        // Receiving request:
        printf("\nReceiving request...");
        receiveFile(sockfd, (char*) REQUEST_SERVER_FILE);

        // calls upper layers:
        printf("\nSending request to upper layers...\n");
        sockfdil = connectSocket("localhost", INTERNET_PORT_SERVER);
        strcpy(buffer,"Request received!\n");
        sendMessage(sockfdil, buffer);

        // After upper layers complete processing, get response:
        printf("\nReceiving response...");
        receiveMessage(sockfdil, buffer);
        printf("\nMessage received!");//: %s", buffer);

        // This simulates physical layer Sending package though medium...
        writeFile(buffer, strlen(buffer), RESPONSE_SERVER_FILE);
        // Send file to client physical layer
        sendFile(sockfd, RESPONSE_SERVER_FILE, this_mac, cli_mac);
	    remove( RESPONSE_SERVER_FILE );

        close(sockfd);
        printf("\nDONE!\n\n");
        // return to loop and wait for next connection
    }
    return 0;
}
