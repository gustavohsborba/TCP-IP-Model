//
// Created by gustavo on 06/10/16.
//

#ifndef PHYSICAL_CPP_PHYSICAL_H
#define PHYSICAL_CPP_PHYSICAL_H



// Para poder fazer if's com uma linha só:
void error(const char *msg){
    perror(msg);
    exit(1);
}









#define MIN_MSG_BUFF 256
#define MIN_MSG_SIZE 255
#define PORT_NUMBER 8761
#define MAX_BUF	    8191
#define BUF_SIZ     8192


#define PREAMBLE_SIZE 8
#define MAC_SIZE      6
#define PROTOCOL_SIZE 2
#define CHECKSUM_SIZE 4
#define MAX_DATA_SIZE MAX_BUF-PREAMBLE_SIZE-MAC_SIZE-PROTOCOL_SIZE-CHECKSUM_SIZE



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




void createFrame(struct Frame *frame, char *message, char* src_mac, char* dst_mac){
    strcpy (frame->preamble, "\x03\x21\x41\x3f\x04\x21\x41\x3f"); // pre-defined preamble.
    strcpy (frame->sourceMAC, src_mac);
    strcpy (frame->destinationMAC, dst_mac);
    strcpy (frame->ethernetType, "\x08\x00"); // type of protocol - 0x0800 for IPv4
    sprintf(frame->data, "%s", message);
    strcpy (frame->checksum, "\x30\x21\x00\x00"); // no checksum calculation. Ain't nobody got time fo dat
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
    strncpy (frame->preamble, buffer, PREAMBLE_SIZE);
    strncpy (frame->sourceMAC, buffer+8, MAC_SIZE);
    strncpy (frame->destinationMAC, buffer+14, MAC_SIZE);
    strncpy (frame->ethernetType, buffer+20, PROTOCOL_SIZE);
    strncpy (frame->data, buffer+22, MAX_DATA_SIZE);
    strncpy (frame->checksum, buffer, CHECKSUM_SIZE);
    printf("Frame Received!!! (%d bytes) \n", (int) n);
    printFrame(frame);
}


void sendFrame(struct Frame *frame, int socket, size_t frame_size){
    ssize_t sent_bytes;
    sent_bytes = send(socket, frame, frame_size, 0);
    if (sent_bytes < 0)
        error("ERROR writing to socket");
    printf("Frame sent! (%d bytes)\n", (int) sent_bytes );
    printFrame(frame);
}






/*
 * sendMessage and receiveMessage
 *
 */



void sendMessage(int sockfd, char *msg){
    char buffer[MIN_MSG_SIZE];
    ssize_t n;
    bzero(buffer,MIN_MSG_BUFF);
    strcpy(buffer, msg);
    n = write(sockfd,buffer,strlen(buffer));
    if (n < 0)
        error("ERROR sending to socket");
    printf("%d bytes sent to client: %s\n", (int)n, buffer);
}

void receiveMessage(int sockfd, char *msg){
    char buffer[MIN_MSG_SIZE];
    ssize_t n;
    bzero(buffer,MIN_MSG_BUFF);
    n = read(sockfd,buffer,MIN_MSG_SIZE);
    if (n < 0)
        error("ERROR reading from socket");
    printf("%d bytes received from client: %s\n", (int) n, buffer);
    strcpy(msg, buffer);
}






#endif //PHYSICAL_CPP_PHYSICAL_H
