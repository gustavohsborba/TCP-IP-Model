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



    char *hostnameOrIp, src_mac[MAC_SIZE], dst_mac[MAC_SIZE];



void receiveFile(int sockfd, const char *filename){
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



void sendFile(int sockfd, const char *filename){
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
        createFrame(&frame, buffer, src_mac, dst_mac);
        printf("\nsending message of %d bytes to server...\n", (int)strlen(frame.data));
        sendFrame(&frame, sockfd, frameSize(&frame));
        bzero(buffer,BUF_SIZ);
        nbytes = fread(buffer, sizeof(char), MAX_DATA_SIZE, msgFile);
    }
    bzero(buffer,BUF_SIZ);
    createFrame(&frame, buffer, src_mac, dst_mac);
    sendFrame(&frame, sockfd, frameSize(&frame));
    printf("file sent.\n");
    fclose(msgFile);
}






int connectSocket(char *hostnameOrIp, char src_mac[MAC_SIZE], char dst_mac[MAC_SIZE]){
    int sockfd; // socket file descriptor
    struct hostent *server;
    struct sockaddr_in serv_addr;
    struct ifreq ifr;

    // Opening socket to start connection:
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");
    // resolving host:
    server = gethostbyname(hostnameOrIp);
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

    // Cheating to get server's MAC addres
    // THE CORRECT WAY IS USING ARP COMMAND.
    /*char mac[10];
    bzero(mac,10);
    getMAC(mac);
    mac[6] = '\0';
    sendMessage(sockfd, mac);
    strncpy(src_mac, mac, 6);
    receiveMessage(sockfd, mac);
    strncpy(dst_mac, mac, 6);*/
	
    return sockfd;
}





int main(int argc, char *argv[])
{
	long SIZE;
    int sockfd, n;
    char *filename;
    struct Frame frame = {};
    char buffer[BUF_SIZ];


    if (argc < 2) {
       fprintf(stderr,"usage %s IP_or_hostname filename\n", argv[0]);
       exit(0);
    }
    hostnameOrIp = argv[1];
    filename = argv[2];


    sockfd = connectSocket(hostnameOrIp, src_mac, dst_mac);
    printf("Passou por aqui!");

    // Connection stabilished. Sending message requesting frame size:
    strcpy(buffer,"Yo, bro! What's the Frame size?");
    sendMessage(sockfd, buffer);
    printf("Passou por aqui!");

    // Reading message from server:
    receiveMessage(sockfd, buffer);
    printf("Message size: %s\n", buffer);
    SIZE = strtol (buffer,NULL,10);
    printf("Passou por aqui!");


    // with socket size negotiated, send filename:
    strcpy(buffer,filename);
    createFrame(&frame, buffer, src_mac, dst_mac);
    sendFrame(&frame, sockfd, frameSize(&frame));

    // And receiving confirmation:
    receiveFrame(&frame, sockfd);
    strcpy(buffer, frame.data);
    printf("Return from server: %s (%d bytes)\n", buffer, (int) frameSize(&frame));


    sendFile(sockfd,filename);

    receiveFile(sockfd,"response.txt");

    close(sockfd);
    return 0;
}












