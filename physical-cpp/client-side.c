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


#define MIN_MSG_BUFF 256
#define MIN_MSG_SIZE 255
#define PORT_NUMBER 8766
//#define MAX_BUF    8191
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


int createFrame(struct Frame *frame, char *message, char* src_mac, char* dst_mac){
    strcpy (frame->preamble, "\x03\x21\x41\x3f\x04\x21\x41\x3f"); // pre-defined preamble.
    strcpy (frame->sourceMAC, src_mac);
    strcpy (frame->destinationMAC, dst_mac);
    strcpy (frame->ethernetType, "\x08\x00"); // type of protocol - 0x0800 for IPv4
    sprintf(frame->data, "%s", message);
    strcpy (frame->checksum, "\x30\x21\x00\x00"); // no checksum calculation. Ain't nobody got time fo dat
    return 8 + 6 + 6 + 2 + 4 + strlen(message);
}

void sendFrame(struct Frame *frame, int socket, int frame_size){
    int sent_bytes;
    sent_bytes = send(socket, frame, frame_size, 0);
    fprintf(stdout, "Client sent %d bytes of %d to the server\n", (int) sent_bytes, frame_size);
    if (sent_bytes < 0)
        error("ERROR writing to socket");
}

void printFrame(struct Frame *frame){
    printf("Frame:\n\tpreamble: %-8s\n",frame->preamble);
    printf("\tsourceMAC: %-6s\n",frame->sourceMAC);
    printf("\tdestinationMAC: %-6s\n",frame->destinationMAC);
    printf("\tethernetType: %-2s\n",frame->ethernetType);
    printf("\tdata: %s\n",frame->data);
    printf("\tchecksum: %s\n",frame->checksum);
}






int connectSocket(char *hostnameOrIp, char **src_mac, char **dst_mac){
    int sockfd; // socket file descriptor
    struct hostent *server;
    struct sockaddr_in serv_addr;
    struct ifreq ifr;

    char src[6];

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


    //retrieve ethernet interface index
    strncpy(ifr.ifr_name, "eth0", IFNAMSIZ);
    if (ioctl(sockfd, SIOCGIFINDEX, &ifr) == -1)
        error("SIOCGIFINDEX");

    int ifindex = ifr.ifr_ifindex;
    //retrieve corresponding MAC
    if (ioctl(sockfd, SIOCGIFHWADDR, &ifr) == -1)
        error("SIOCGIFHWADDR");

    for (int i = 0; i < 6; i++) {
        src[i] = ifr.ifr_hwaddr.sa_data[i];
    }

    *src_mac = "abcdef";
    *dst_mac = "abcdef";

    //*src_mac = src;
    //printf("Successfully got our MAC address: %02X:%02X:%02X:%02X:%02X:%02X\n",
    //       src_mac[0],src_mac[1],src_mac[2],src_mac[3],src_mac[4],src_mac[5]);
    return sockfd;
}





int main(int argc, char *argv[])
{
	int MAX_BUF;
    int sockfd,  n;
    char *hostnameOrIp, *src_mac, *dst_mac;
    char *filename;
    struct Frame frame = {};
    char buffer[MIN_MSG_BUFF];


    if (argc < 2) {
       fprintf(stderr,"usage %s IP_or_hostname filename\n", argv[0]);
       exit(0);
    }
    hostnameOrIp = argv[1];
    filename = argv[2];

    
    sockfd = connectSocket(hostnameOrIp, &src_mac, &dst_mac);

    // Connection stabilished. Sending message requesting frame size:
    bzero(buffer,MIN_MSG_BUFF);
    strcpy(buffer,"Yo, bro! What's the Frame size?");
    n = write(sockfd,buffer,strlen(buffer));

    // Reading message from server: 
    bzero(buffer,MIN_MSG_BUFF);
    n = read(sockfd,buffer,MIN_MSG_SIZE);
    if (n < 0) 
         error("ERROR reading from socket");
    printf("Message size: %s\n", buffer);
    MAX_BUF = strtol (buffer,NULL,10);




    // with socket size negotiated, send filename:
    //n = createFrame(&frame, filename, src_mac, dst_mac);
    //printFrame(&frame);
    //sendFrame(&frame, sockfd, n);
    bzero(buffer,MIN_MSG_BUFF);
    strcpy(buffer,filename);
    n = write(sockfd,buffer,strlen(filename));
    bzero(buffer,MIN_MSG_BUFF);
    n = read(sockfd,buffer,MIN_MSG_SIZE);
    printf("Return from server: %s (%d bytes)\n", buffer, n);
    if(n < 9)
        error("Server is not OK!");
    
    
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












