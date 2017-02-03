import socket
import sys
import os

LOCALHOST = "127.0.0.1"
TRANSPORT_PORT_SERVER = 31111;
TRANSPORT_PORT_CLIENT = 31112;
APPLICATION_PORT_SERVER = 41111;
APPLICATION_PORT_CLIENT = 41112;
MAX_BUF = 65536;


def send_and_receive_from_application_layer(data):
    # Connect with application layer and send request data:
    applayer_address = (LOCALHOST, APPLICATION_PORT_SERVER)
    print >>sys.stderr, 'Trying to connect to application layer on %s port %s' % applayer_address
    appl_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    appl_sock.connect(applayer_address)
    print >>sys.stderr, 'Sending data to application layer...'        
    appl_sock.sendall(data)
    appl_sock.send("\n")
    appl_sock.send("")
    sockfd = os.fdopen(appl_sock.fileno())
    sockfd.flush()

    #Getting application layer response and sending back to internet layer
    print >>sys.stderr, 'Data sent. Receiving response...'
    data = appl_sock.recv(MAX_BUF)
    sockfd.close()
    return data



# Create a TCP/IP socket
net_listener = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
# Bind the socket to the port
netlayer_address = (LOCALHOST, TRANSPORT_PORT_SERVER)
print >>sys.stderr, 'starting up on %s port %s' % netlayer_address
net_listener.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
net_listener.bind(netlayer_address)
# Listen for incoming connections
net_listener.listen(1)

while True:
	
    # Wait for a connection
    print >> sys.stderr, 'waiting for an Internet Layer connection'
    net_sock, client_address = net_listener.accept()

    # Accept internet layer connection and receive request
    print >> sys.stderr, 'connection from internet layer: ', client_address
    data = net_sock.recv(MAX_BUF)
    print >>sys.stderr, 'data:\n\t"%s"' % data

    # ##############################################
    # SOME TRANSPORT LAYER PROCESSING HERE!!!
    splitdata = data.split(',')
    data = splitdata[-1]
    origport = splitdata[0].split(":")[1]
    destport = splitdata[1].split(":")[1]

    if 'SYN' in data:
        data = 'SYNACK'
    elif 'FIN' in data:
        data = 'ACK'
    else:
        data = send_and_receive_from_application_layer(data)

    data = "origport:" + str(destport) + ",destport:" + str(origport) + ',' + data;
    #  ##############################################

    print >> sys.stderr, '\nResponse:\n%s\n\n' % data
    print >>sys.stderr, 'sending it to Internet layer...'
    sent = net_sock.send(data)
    sent = net_sock.send("\n")
    sent = net_sock.send("")
    
    print >>sys.stderr, '\n\nAnother job well-done!!! Closing connection...\n\n\n'
    sockfd = os.fdopen(net_sock.fileno())
    sockfd.flush()
    net_sock.close()
    