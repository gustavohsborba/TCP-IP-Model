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
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/sendfile.h>
#include <netinet/in.h>
#include <netdb.h>

#include <sys/ioctl.h>
#include <linux/if_arp.h>

#include <pthread.h>
#include <time.h>

#include "physical.h"



int connectSocket(char *hostnameOrIp, char **src_mac, char **dst_mac){
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

    *src_mac = "abcdef";
    *dst_mac = "abcdef";

    return sockfd;
}





int main(int argc, char *argv[])
{
	long SIZE;
    int sockfd,  n;
    char *hostnameOrIp, *src_mac, *dst_mac;
    char *filename;
    struct Frame frame = {};
    char buffer[MAX_BUF];


    if (argc < 2) {
       fprintf(stderr,"usage %s IP_or_hostname filename\n", argv[0]);
       exit(0);
    }
    hostnameOrIp = argv[1];
    filename = argv[2];


    sockfd = connectSocket(hostnameOrIp, &src_mac, &dst_mac);

    // Connection stabilished. Sending message requesting frame size:
    strcpy(buffer,"Yo, bro! What's the Frame size?");
    sendMessage(sockfd, buffer);

    // Reading message from server:
    receiveMessage(sockfd, buffer);
    printf("Message size: %s\n", buffer);
    SIZE = strtol (buffer,NULL,10);


    // with socket size negotiated, send filename:
    strcpy(buffer,filename);
    createFrame(&frame, buffer, src_mac, dst_mac);
    sendFrame(&frame, sockfd, frameSize(&frame));

    // And receiving confirmation:
    receiveFrame(&frame, sockfd);
    strcpy(buffer, frame.data);
    printf("Return from server: %s (%d bytes)\n", buffer, (int) frameSize(&frame));


    // Opening message file to read bytes and send them:
    FILE* msgFile;
    msgFile = fopen(filename,"rb");
    int msgFd = fileno(msgFile);
    if( !msgFile | msgFd < 0)
    	error("File doesn't exist");

    // Finally, Sending files:
    int i = 0;
    int c;
    bzero(buffer,MIN_MSG_BUFF);
    size_t nbytes = fread(buffer, sizeof(char), MIN_MSG_BUFF, msgFile);
    while (nbytes > 0){
        createFrame(&frame, buffer, src_mac, dst_mac);
        sendFrame(&frame, sockfd, frameSize(&frame));
        //sendMessage(sockfd, buffer);
        nbytes = fread(buffer, sizeof(char), MIN_MSG_BUFF, msgFile);
    }
    printf("File sent.\n");

    fclose(msgFile);
    close(sockfd);
    return 0;
}












