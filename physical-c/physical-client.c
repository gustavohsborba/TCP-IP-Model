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
    socklen_t network_len;
    
    char buffer[BUF_SIZ];
    char *hostnameOrIp, src_mac[MAC_SIZE], dst_mac[MAC_SIZE];
    char *temp, *rawrequest;
    struct Frame frame = {};
    
    listener = startListening(INTERNET_PORT_CLIENT);
    
    while(1){
        printf("\nPhysical layer listening to upper layers...");
        sockfdil = accept(listener, (struct sockaddr *) &network_layer,  &network_len);
        printf("\nConnection stabilished!");

        // reads network package and finds destiny IP inside it. 
        printf("\nGetting request...");
        bzero(&buffer,sizeof(buffer));
        bread=recv(sockfdil,buffer,MAX_BUF,0) <= 0;
        strcpy(rawrequest, buffer);
        temp=strtok(buffer," ");
        while(temp != NULL){
            if(strstr(temp,"Host:") == temp){
                strtok(temp,":");
                hostnameOrIp=strtok(NULL,":");
                break;
            }
            printf("temp: %s\n", temp);
            temp=strtok(NULL," ");
        }
        printf("%s\n", rawrequest);
        printf("\nDestiny IP found: %s", hostnameOrIp);
        
        // This simulates physical layer Sending package though medium...
        writeFile(rawrequest, strlen(buffer), REQUEST_CLIENT_FILE);
        
        // Connecting to server socket:
        printf("\nConnecting to server...");
        sockfd = connectSocket(hostnameOrIp, PORT_NUMBER);


        // Cheating to get server's MAC addres
        // THE CORRECT WAY IS USING ARP COMMAND.
        bzero(buffer,10);
        getMAC(buffer);  // getting own mac
        buffer[6] = '\0';
        sendMessage(sockfd, buffer); // sending mac to server
        strncpy(src_mac, buffer, 6);
        receiveMessage(sockfd, buffer); // receiving server's mac
        strncpy(dst_mac, buffer, 6);


        // Connection stabilished. Sending message requesting frame size:
        printf("\nNegotiating Frame Size....");
        strcpy(buffer,"Yo, bro! What's the Frame size?");
        sendMessage(sockfd, buffer);

        // Reading message from server:
        receiveMessage(sockfd, buffer);
        printf("Message size: %s\n", buffer);
        
        
        
        // Finally Sends request from the file created (physical medium) frame by frame to server.
        printf("\nSending request...");
        sendFile(sockfd,REQUEST_CLIENT_FILE, src_mac, dst_mac);

        // Receive response frame by frame from server and creates a temp file.
        printf("\nWaiting for response...");
        receiveFile(sockfd,TEMP_FILE);
        // LER FRAMES RECEBIDOS DA CAMADA FÍSICA DO SERVIDOR E ENVIAR PELO SOCKET DE REDE!!!
        strcpy(buffer,"File Received Successfully!\n");
        printf("\n%s", buffer);
        sendMessage(sockfdil, buffer);

        close(sockfd);
        close(sockfdil);
        printf("\nDONE!\n\n");
    }
    return 0;
}












