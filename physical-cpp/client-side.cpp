/*
 * Simulates a physical layer from the TCP/IP model.
 * Following this tutorial: http://www.linuxhowtos.org/C_C++/socket.htm
 * possibly, the following too: http://www.dicas-l.com.br/arquivo/programando_socket_em_c++_sem_segredo.php#.V9lODHUrLDc
 */

//#include<bits/stdc++.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <pthread.h>
#include <time.h>


/* sockaddr_in is defined in netinet/in.h:
 * struct sockaddr_in {
 *     short   sin_family; // must be AF_INET
 *     u_short sin_port;
 *     struct  in_addr sin_addr;
 *     char    sin_zero[8]; // Not used, must be zero
 * };
 */

/* hostent is a sctructure defined in netdb.h:
 * struct  hostent {
 *     char    *h_name;        // official name of host
 *     char    **h_aliases;    // alias list. A zero  terminated  array  of
 *                             // alternate names for the host.
 *     int     h_addrtype;     // host address type. The  type  of  address
 *                             // being  returned. currently always AF_INET.
 *     int     h_length;       // length of address
 *     char    **h_addr_list;  // list of addresses from name server
 *     #define h_addr  h_addr_list[0]  // address, for backward compatiblity.
 *                             //A pointer to a list of network addresses
 *                             // for the named host.  Host addresses are
 *                             // returned in network byte order.
 * };
 */


class Frame{
    	unsigned char sourceMAC[6]; // source's MAC address (6 octets)
    	unsigned char destinationMAC[6]; // destination's MAC address (6 octets)
    	unsigned char protocol[2]; // type of protocol - 0x0800 for IPv4
    	unsigned char data[1002]; // 1006 bytes limit for data
    	unsigned char checksum[8]; // to check for transmission errors
    public:
        Frame(char src_mac[], char dst_mac[], char message[]) {
        	strcpy(sourceMAC, src_mac);
        	strcpy(destinationMAC, dst_mac);
        	strcpy(protocol, "\x08\x00");
        	sprintf(data, "%-494s", message);
        	strcpy(checksum, "\x00\x20\x20\x3a");
        }
        ~Frame()
};

// Para poder fazer if's com uma linha só:
void error(char *msg){
    perror(msg);
    exit(1);
}



int main(int argc, char * argv[]){
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    char buffer[1024];




    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");


    // A partir daqui, não sabemos o que estamos fazendo...


    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
        error("ERROR connecting");
    printf("Please enter the message: ");
    bzero(buffer,256);
    fgets(buffer,255,stdin);
    n = write(sockfd,buffer,strlen(buffer));
    if (n < 0)
         error("ERROR writing to socket");
    bzero(buffer,256);



}

using namespace std;