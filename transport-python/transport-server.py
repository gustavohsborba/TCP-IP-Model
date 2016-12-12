import socket
import sys

LOCALHOST = "localhost"
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
        print >>sys.stderr, 'received:\n\t"%s"' % data

        # Connect with application layer and send request data:
        applayer_address = (LOCALHOST, APPLICATION_PORT_SERVER)
        appl_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        appl_sock.connect(applayer_address)
        print >>sys.stderr, 'Sending data to application layer...\n"'        
        appl_sock.sendall(data)
        
        #Getting application layer response and sending back to internet layer
        print >>sys.stderr, 'Data sent. Receiving response...\n"'
        data = appl_sock.recv(MAX_BUF)
        print >>sys.stderr, 'sending to transport layer...\n"'
        net_sock.sendall(data)

        appl_sock.close()
    finally:
        # Clean up the connection
        net_sock.close()