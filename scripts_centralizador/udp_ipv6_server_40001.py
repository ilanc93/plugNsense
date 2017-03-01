import socket
import multiprocessing
import data_processor
import ipaddress #python 3 module
import os
  
UDP_IP = "::" # = 0.0.0.0 u IPv4

#Usamos el puerto 40000 para recibir datos y mandar a DB
UDP_PORT = 40001 

sock = socket.socket(socket.AF_INET6, # Internet
						socket.SOCK_DGRAM) # UDP
sock.bind((UDP_IP, UDP_PORT))

while True:
	data, addr = sock.recvfrom(1024) 
	address = str(addr[0])
	formated_address = address.split('%')[0]
	
	addr = ipaddress.IPv6Address(formated_address)
	#print ("UDP 40001 %s" % str(data.decode("utf-8") ))
	p = multiprocessing.Process(target=os.system("./nat_bridge_out.py %s" % str(data.decode("utf-8")) ))
	
	p.start()
