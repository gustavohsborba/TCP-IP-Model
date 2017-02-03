import socket
import sys
import os

LOCALHOST = "127.0.0.1"
TRANSPORT_PORT_CLIENT = 31112;
INTERNET_PORT_CLIENT = 21112;
MAX_BUF = 65536;




def send_and_receive_from_network(rawdata, seq, ack):
    origport = os.getpid()
    destport = 8080
    
    data = "origport:" + str(origport) + ",destport:" + str(destport)
    data = data + ',' + 'seq:' + seq+ ',ack:' + ack+ ',data:' + rawdata;
    print >>sys.stderr, 'Data:\n\t"%s"' % data

    # Connect with Internet Layer and send request data:
    netlayer_address = (LOCALHOST, INTERNET_PORT_CLIENT)
    print >>sys.stderr, 'Trying to connect to Internet Layer on %s port %s' % netlayer_address
    net_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    net_sock.connect(netlayer_address)
    print >>sys.stderr, 'Sending data to Internet Layer...'        
    net_sock.sendall(data)
    net_sock.send("\n")
    net_sock.send("")
    sockfd = os.fdopen(net_sock.fileno())
    sockfd.flush()
    
    #Getting Internet Layer response and sending back to Application Layer
    print >>sys.stderr, 'Data sent. Receiving response...'
    data = net_sock.recv(MAX_BUF)

    print >> sys.stderr, '\nResponse:\n%s\n\n' % data
    splitdata = data.split(',')
    data = splitdata[-1]
    sockfd.close()
    return data


# Create a TCP/IP socket
appl_listener = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
# Bind the socket to the port
applayer_address = (LOCALHOST, TRANSPORT_PORT_CLIENT)
print >>sys.stderr, 'starting up on %s port %s' % applayer_address
appl_listener.bind(applayer_address)
# Listen for incoming connections
appl_listener.listen(5)

while True:
    
    # Wait for a connection
    print >> sys.stderr, 'waiting for an Application Layer connection'
    appl_sock, app_address = appl_listener.accept()
    
    
    # Accept Application Layer connection and receive request
    print >> sys.stderr, 'connection from Application Layer: ', app_address
    rawdata = appl_sock.recv(MAX_BUF)
    host = rawdata.split(':')[-1]

    # ##############################################
    # SOME TRANSPORT LAYER PROCESSING HERE:
    
    # FIRST: Send SYN
    syn = 'SYN Host:' + host
    # SECOND: Receive SYN + ACK com Frame size
    synack = send_and_receive_from_network(syn, '01', '01')
    # THIRD: Send SYN + package, FOURTH: Receive ACK with response...
    response = send_and_receive_from_network(rawdata, '03', '02')
    # FIFTH: Send FIN
    fin = 'FIN Host:' + host
    # SEVENTH: receive ACK
    data = send_and_receive_from_network(fin, '05', '04')
    # GOT full data.

    print >>sys.stderr, 'sending it to Application Layer...'
    sent = appl_sock.send(response)
    sent = appl_sock.send("\n")
    sent = appl_sock.send("")
    sockfd = os.fdopen(appl_sock.fileno())
    sockfd.flush()
    sockfd.close()

    print >>sys.stderr, '\n\nAnother job well-done!!! Closing connection...\n\n\n'
    