import socket
import sys
import subprocess

# Create a TCP/IP socket
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# Connect the socket to the port where the server is listening
server_address = (sys.argv[1], 10000)
print >>sys.stderr, 'connecting to %s port %s' % server_address
#sock.connect(server_address)

try:
    
    # Calling physical layer
    subprocess.check_output(['./physical-client', sys.argv[1], sys.argv[2]])
    print >>sys.stderr, 'Transport sending to physical'

finally:
    print >>sys.stderr, 'closing socket'
    #sock.close()	
