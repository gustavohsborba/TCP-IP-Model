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

#include "physical.h"



int main(int argc, char *argv[])
{
	int sockfd; // socket file descriptor, auxiliary n 
    int sockfdil;  // socket file descriptor of the internet layer
    int listener;  // Internet Layer listener, to send requests.
    int bread;    // number of bytes read
    struct sockaddr_in network_layer;
    socklen_t network_len = 0;
    
    char buffer[BUF_SIZ];
    char internalBuffer[MAX_FILESIZE], rawrequest[MAX_FILESIZE];
    char *hostnameOrIp, src_mac[MAC_SIZE], dst_mac[MAC_SIZE];
    char *temp;
    struct Frame frame = {};
    
    printf("Client Physical layer started!\n");
    listener = startListening(PHYSICAL_PORT_CLIENT);
    
    while(1){
        printf("Physical layer listening to upper layers on port %d...\n", PHYSICAL_PORT_CLIENT);
        sockfdil = accept(listener, (struct sockaddr *) &network_layer,  &network_len);
        printf("Connection stabilished!\n");

        // reads network package and finds destiny IP inside it. 
        printf("Getting request from internet layer...\n");
        bzero(internalBuffer,MAX_FILESIZE);
        bread=recv(sockfdil,internalBuffer,MAX_FILESIZE,0) <= 0;
        strncpy(rawrequest, internalBuffer, MAX_FILESIZE);
        temp=strtok(internalBuffer,"|");
        while(temp != NULL){
            if(strstr(temp,"ipdest:") == temp){
                strtok(temp,":");
                hostnameOrIp=strtok(NULL,":");
                hostnameOrIp=trim(hostnameOrIp);
                break;
            }
            printf("temp: %s\n", temp);
            temp=strtok(NULL," ");
        }
        
        // This simulates physical layer Sending package though medium...
        writeFile(rawrequest, strlen(rawrequest), REQUEST_CLIENT_FILE);
        
        // Connecting to server socket:
        printf("Connecting to server %s on port %d...\n", hostnameOrIp, PORT_NUMBER);
        sockfd = connectSocket(hostnameOrIp, PORT_NUMBER);

        // Cheating to get server's MAC addres
        // THE CORRECT WAY IS USING ARP COMMAND.
        printf("Getting MAC Addresses...\n");
        bzero(buffer,10);
        getMAC(buffer);  // getting own mac
        buffer[6] = '\0';
        sendMessage(sockfd, buffer); // sending mac to server
        strncpy(src_mac, buffer, 6);
        receiveMessage(sockfd, buffer); // receiving server's mac
        strncpy(dst_mac, buffer, 6);

        // Connection stabilished. Sending message requesting frame size:
        printf("Negotiating Frame Size....\n");
        strcpy(buffer,"Yo, bro! What's the Frame size?");
        sendMessage(sockfd, buffer);

        // Reading message from server:
        receiveMessage(sockfd, buffer);
        printf("Message size: %s\n", buffer);
        
        // Finally Sends request from the file created (physical medium) frame by frame to server.
        printf("Sending request to server...\n");
        sendFile(sockfd,REQUEST_CLIENT_FILE, src_mac, dst_mac);
        remove( REQUEST_CLIENT_FILE );

        // Receive response frame by frame from server and creates a temp file.
        printf("Waiting for server response...\n");
        receiveFile(sockfd,RESPONSE_CLIENT_FILE);
        
        // Read response from the temp file and 
        readFile(internalBuffer, RESPONSE_CLIENT_FILE);

        // Send response trough internet layer socket
        printf("Sending response to Internet Layer...\n");
        sendMessage(sockfdil, internalBuffer);
        receiveMessage(sockfdil, internalBuffer);
        remove( RESPONSE_CLIENT_FILE );

        close(sockfd);
        close(sockfdil);
        printf("\nDONE!\n\n");
    }
    return 0;
}












