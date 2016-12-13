//
// Created by gustavo on 06/10/16.
//

#ifndef PHYSICAL_CPP_PHYSICAL_H
#define PHYSICAL_CPP_PHYSICAL_H



#include <stdio.h>
#include <ctype.h>
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






// Just encapsulating a command to make if's in one single line:
void error(const char *msg){
    perror(msg);
    exit(1);
}







//#define NETWORK_INTERFACE "wlp1s0"
#define NETWORK_INTERFACE "wlp7s0"
//"ethZero0 TCHARAM"


#define INTERNET_PORT_SERVER 21111
#define INTERNET_PORT_CLIENT 21112
#define PHYSICAL_PORT_SERVER 11111
#define PHYSICAL_PORT_CLIENT 11112
#define PORT_NUMBER           9999
#define MIN_MSG_BUFF          576
#define MIN_MSG_SIZE          MIN_MSG_BUFF - 1
#define BUF_SIZ               576
#define MAX_BUF	              BUF_SIZ - 1
#define MAX_FILESIZE         65536


#define PREAMBLE_SIZE 8
#define MAC_SIZE      6
#define PROTOCOL_SIZE 2
#define CHECKSUM_SIZE 4
#define MAX_DATA_SIZE MAX_BUF-PREAMBLE_SIZE-(2*MAC_SIZE)-PROTOCOL_SIZE-CHECKSUM_SIZE-2

#define TEMP_FILE     "response.txt"
#define RESPONSE_SERVER_FILE "response_physical.srv"
#define REQUEST_SERVER_FILE "request_physical.srv"
#define RESPONSE_CLIENT_FILE "response_physical.cli"
#define REQUEST_CLIENT_FILE  "request_physical.cli"
#define LOCALHOST            "127.0.0.1"



/*************************************************************************************************
 *                                  USEFUL FUNCTIONS                                             *
 *************************************************************************************************/


char *ltrim(char *s) 
{     
    while(isspace(*s)) s++;     
    return s; 
}  

char *rtrim(char *s) 
{     
    char* back;
    int len = strlen(s);

    if(len == 0)
        return(s); 

    back = s + len;     
    while(isspace(*--back));     
    *(back+1) = '\0';     
    return s; 
}  

char *trim(char *s) 
{     
    return rtrim(ltrim(s));  
} 



/*************************************************************************************************
 *                                  SOCKET FUNCTIONS                                             *
 *************************************************************************************************/


int startListening(int port){
    int listener, sockfd; // Socket file descriptors
    struct sockaddr_in serv_addr;
    // Opening socket to listen to connection:
    listener = socket(AF_INET, SOCK_STREAM, 0);
    if (listener < 0)
        error("ERROR opening socket");
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(port);
    if (bind(listener, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR on binding");
    listen(listener,5);
    return listener;
}

int connectSocket(char *hostnameOrIp, int port_number){
    int sockfd; // socket file descriptor
    struct hostent *server;
    struct sockaddr_in serv_addr;
    struct ifreq ifr;

    // Opening socket to start connection:
    printf("\nOpening socket....");
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");
    // resolving host:
    printf("\nResolving Host %s....", hostnameOrIp);
    server = gethostbyname(hostnameOrIp);
    printf("\nHost solved!");
    if (server == NULL)
        error("ERROR, no such host\n");
    printf("\ncleaning serv_addr...");
    bzero((char *) &serv_addr, sizeof(serv_addr)); // cleans serv_addr

    // configuring and connecting socket:
    printf("\nConfiguring and connecting....");
    serv_addr.sin_family = AF_INET;
    bcopy( (char *)server->h_addr,
           (char *)&serv_addr.sin_addr.s_addr,
           server->h_length);
    serv_addr.sin_port = htons(port_number);
    printf("\nConfigured! now connecting....");
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
        error("ERROR connecting");

    return sockfd;
}


/*************************************************************************************************
 *                              FRAME DEFINITION AND FUNCTIONS                                   *
 *************************************************************************************************/


struct Frame{
    char preamble[PREAMBLE_SIZE]; // 8 bytes for preamble
    char sourceMAC[MAC_SIZE]; // source's MAC address
    char destinationMAC[MAC_SIZE]; // destination's MAC address
    char ethernetType[PROTOCOL_SIZE];// type of protocol - 0x0800 for Ethernet in IPv4
    char data[MAX_DATA_SIZE]; // Data has [Frame size - frame items size] size.
    char checksum[CHECKSUM_SIZE];
};


size_t frameSize(struct Frame *frame){
    return PREAMBLE_SIZE + (2*MAC_SIZE)
           + PROTOCOL_SIZE + CHECKSUM_SIZE
           + strlen(frame->data);
}

void getData(struct Frame *frame, char data[MAX_DATA_SIZE+1]){
    bzero(data, MAX_DATA_SIZE+1);
    strncpy(data, frame->data, MAX_DATA_SIZE);
    data[MAX_DATA_SIZE] = '\0';
}



void createFrame(struct Frame *frame, char *message, char* src_mac, char* dst_mac){
    strcpy (frame->preamble, "\xaa\xaa\xaa\xaa\xaa\xaa\xaa\xab"); // pre-defined preamble.
    strcpy (frame->sourceMAC, src_mac);
    strcpy (frame->destinationMAC, dst_mac);
    strcpy (frame->ethernetType, "\x08\x00"); // type of protocol - 0x0800 for IPv4
    bzero  (frame->data, MAX_DATA_SIZE);
    sprintf(frame->data, "%s", message);
    strcpy (frame->checksum, "\x30\x21\x00\x00"); // no checksum calculation. Ain't nobody got time fo dat
}

/*
 * @function bytesToStr
 * Copies P to D as byte notation string
 * P has to be a non-empty string
 * D has to be at least 3 times len (wich is the size of P)
 */
void bytesToStr(char *p, char *d, size_t len){
    char * ptr = d;
    ptr+=sprintf(ptr, "[");
    for (int i = 0; i < len-1; ++i)
        ptr+=sprintf(ptr,"%02x:",(unsigned char) p[i]);
    ptr+=sprintf(ptr,"%02x",(unsigned char)p[len-1]);
    ptr+=sprintf(ptr, "]");
    *(ptr + 1) = '\0';
}


void printFrame(struct Frame *frame){
    char aux[BUF_SIZ];
    bytesToStr(frame->preamble, aux, PREAMBLE_SIZE );
    printf("Frame:\n\tpreamble: %s\n",aux);
    bytesToStr(frame->sourceMAC, aux, MAC_SIZE );
    printf("\tsourceMAC: %s\n",aux);
    bytesToStr(frame->destinationMAC, aux, MAC_SIZE );
    printf("\tdestinationMAC: %s\n",aux);
    bytesToStr(frame->ethernetType, aux, PROTOCOL_SIZE );
    printf("\tethernetType: %s\n",aux);
    getData(frame, aux);
    for(int i=0; i<strnlen(frame->data, MAX_DATA_SIZE); i++)
        if(aux[i]=='\n' || aux[i]=='\t' || aux[i]==' ')
            aux[i] = '_';
    printf("\tdata: [%s]\n",aux);
    bytesToStr(frame->checksum, aux, CHECKSUM_SIZE );
    printf("\tchecksum: %s\n",aux);
}


void printFrame2(struct Frame *frame){
    char aux[BUF_SIZ];
    bytesToStr(frame->preamble, aux, PREAMBLE_SIZE );
    printf("Frame:\n\tpreamble: %s\n",aux);
    bytesToStr(frame->sourceMAC, aux, MAC_SIZE );
    printf("\tsourceMAC: %s\n",aux);
    bytesToStr(frame->destinationMAC, aux, MAC_SIZE );
    printf("\tdestinationMAC: %s\n",aux);
    bytesToStr(frame->ethernetType, aux, PROTOCOL_SIZE );
    printf("\tethernetType: %s\n",aux);
    getData(frame, aux);

    char data[BUF_SIZ];
    int i=0, j=0;
    data[0] = '[';
    for(j=1; i<strnlen(frame->data, MAX_DATA_SIZE); j++){
        for(i=0; i<strnlen(frame->data, MAX_DATA_SIZE); i++)
            if(aux[i]=='\n' || aux[i]=='\t' || aux[i]==' ')
                aux[i] = '_';
        if((j%15)==0) data[j++] = '\t';
        data[j] = aux[i];
    }
    data[j++] = ']';
    data[j] = '\0';
    printf("\tdata: [%s]\n",data);

    bytesToStr(frame->checksum, aux, CHECKSUM_SIZE );
    printf("\tchecksum: %s\n",aux);
}

void receiveFrame(struct Frame *frame, int sockfd){
    char buffer[BUF_SIZ];
    bzero(buffer,BUF_SIZ);
    int n = read(sockfd,buffer,MAX_BUF);
    if (n<0)
        error("Could not read from socket");
    int offset = 0;
    strncpy (frame->preamble, buffer, PREAMBLE_SIZE);
    offset += PREAMBLE_SIZE;
    strncpy (frame->sourceMAC, buffer+offset, MAC_SIZE);
    offset += MAC_SIZE;
    strncpy (frame->destinationMAC, buffer+offset, MAC_SIZE);
    offset += MAC_SIZE;
    strncpy (frame->ethernetType, buffer+offset, PROTOCOL_SIZE);
    offset += PROTOCOL_SIZE;
    bzero(frame->data, MAX_DATA_SIZE);
    strncpy (frame->data, buffer+offset, MAX_DATA_SIZE);
    frame->data[MAX_DATA_SIZE] = '\0';
    offset = n -CHECKSUM_SIZE;
    strncpy (frame->checksum, buffer+offset, CHECKSUM_SIZE);
    if(n!=0){
        printf("Frame Received!!! (%d bytes) \n", (int) n);
        printFrame(frame);
    }
}


void sendFrame(struct Frame *frame, int socket, size_t frame_size){
    ssize_t sent_bytes;
    sent_bytes = send(socket, (int*)frame, frame_size, 0);
    if (sent_bytes < 0)
        error("ERROR writing to socket");
    printf("Frame sent! (%d bytes)\n", (int) sent_bytes );
    printFrame(frame);
}






/*************************************************************************************************
 *                                  MESSAGE  FUNCTIONS                                           *
 *************************************************************************************************/
 
 
void sendMessage(int sockfd, char *msg){
    char buffer[MAX_FILESIZE];
    ssize_t n;
    strcpy(buffer, msg);
    n = write(sockfd,buffer,strlen(buffer));
    if (n < 0)
        error("ERROR sending to socket");
    printf("%d bytes sent.\n", (int)n);
}

void receiveMessage(int sockfd, char *msg){
    char buffer[MAX_FILESIZE];
    ssize_t n;
    bzero(buffer,MAX_FILESIZE);
    n = read(sockfd,buffer,MAX_FILESIZE);
    if (n < 0)
        error("ERROR reading from socket");
    printf("%d bytes received: %s\n", (int) n, buffer);
    strcpy(msg, buffer);
}



void getMAC(char mac[MAC_SIZE]){
    int s;
    struct ifreq buffer;

    s = socket(PF_INET, SOCK_DGRAM, 0);
    memset(&buffer, 0x00, sizeof(buffer));
    strcpy(buffer.ifr_name, NETWORK_INTERFACE);
    ioctl(s, SIOCGIFHWADDR, &buffer);
    close(s);

    for( s = 0; s < 6; s++ )
        mac[s] = (char) buffer.ifr_hwaddr.sa_data[s];
}





/*************************************************************************************************
 *                       'SENDING FILES THROUGH FRAMES' FUNCTIONS                                *
 *************************************************************************************************/

void writeFile(char *buffer, int size, char *filename){
    FILE* file;
    int fd;
    file = fopen(filename,"wb");
    fd = fileno(file);
    if( !file | fd < 0)
        error("Couldn't create file!");
    size_t len = strlen(buffer);
    printf("Sending data through physical medium...\n\n");
    if ((int) len <= 0) error("LEN <= 0 from WRITE FILE");
    else fwrite(buffer, sizeof(char), len, file);
    fclose(file);
}

int readFile(char *buffer, char *filename){
    FILE* file;
    int fd;
    file = fopen(filename,"rb");
    fd = fileno(file);
    if( !file | fd < 0)
        error("Couldn't create file!");
    bzero(buffer, MAX_FILESIZE);
    fseek (file, 0, SEEK_END);
    int length = ftell (file);
    fseek (file, 0, SEEK_SET);

    printf("\nReading data from physical medium...\n");
    size_t size = fread(buffer, sizeof(char), length, file);
    buffer[size] = '\0';
    printf("%d bytes Read.\n", (int)size);
    fclose(file);
    return (int) size;
}


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



void sendFile(int sockfd, const char *filename, char* src_mac, char* dst_mac){
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



#endif //PHYSICAL_CPP_PHYSICAL_H
