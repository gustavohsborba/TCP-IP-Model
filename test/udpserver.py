import socket
import sys
import subprocess

try:
    
    # calling application layer
    subprocess.check_output(["scala" , "application-server.scala"])
	#subprocess.check_output(['./abacaxi'])
    print >>sys.stderr, 'Transport sending to application'

finally:
    print >>sys.stderr, 'closing socket udp server'
