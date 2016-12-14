import socket
import sys
import os

LOCALHOST = "127.0.0.1"
TRANSPORT_PORT_SERVER = 31111;
TRANSPORT_PORT_CLIENT = 31112;
APPLICATION_PORT_SERVER = 41111;
APPLICATION_PORT_CLIENT = 41112;
MAX_BUF = 65536;

# Create a TCP/IP socket
net_listener = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
# Bind the socket to the port
netlayer_address = (LOCALHOST, TRANSPORT_PORT_SERVER)
print >>sys.stderr, 'starting up on %s port %s' % netlayer_address
net_listener.bind(netlayer_address)
# Listen for incoming connections
net_listener.listen(1)

while True:
    
    # Wait for a connection
    print >> sys.stderr, 'waiting for an Internet Layer connection'
    net_sock, client_address = net_listener.accept()
	
    try:
        # Accept internet layer connection and receive request
        print >> sys.stderr, 'connection from internet layer: ', client_address
        data = net_sock.recv(MAX_BUF)
        print >>sys.stderr, 'data:\n\t"%s"' % data

        # ##############################################
        # DO SOME TRANSPORT LAYER PROCESSING HERE!!!
        # ##############################################
        splitdata = data.split(',')
        data = splitdata[-1]
        origport = splitdata[0].split(":")[1]
        destport = splitdata[1].split(":")[1]


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

        # ##############################################
        # DO SOME TRANSPORT LAYER PROCESSING HERE!!!
        # ##############################################
        data = "origport:" + str(destport) + ",destport:" + str(origport) + ',' + data;

        print >> sys.stderr, '\nResponse:\n%s\n\n' % data
        print >>sys.stderr, 'sending it to Internet layer...'
        sent = net_sock.send(data)
        sent = appl_sock.send("\n")
        sent = appl_sock.send("")
        sockfd = os.fdopen(appl_sock.fileno())
        sockfd.flush()
        
        print >>sys.stderr, '\n\nAnother job well-done!!! Closing connection...\n\n\n'
        appl_sock.close()
    finally:
        # Clean up the connection
        net_sock.close()