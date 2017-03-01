import socket
import json
from scapy.all import *

#UDP_IP = bytes("fe90::0001",'UTF-8')  # localhost
#UDP_IP = "fe90::1"  
#UDP_IP = "130.0.0.4"
#UDP_PORT = 40001

IP_SRC = "fe80::10"


MESSAGE_CONF = json.dumps({"type":"configuration","data": [{"local_id":2,"io_type":1},{"local_id":1,"io_type":2},{"local_id":3,"io_type":3}]})	

MESSAGE_MEASURE_1 = json.dumps({"type":"measurements","local_id":2,"data": [10.234]})

MESSAGE_MEASURE_X = json.dumps({"type":"measurements","timestamp":["2016-12-01 00:34:27","2016-12-01 00:34:31","2016-12-01 00:34:35"],"local_id":1,"data": [10.234,11.233,14.336]})

MESSAGE_DIRECTIVE = json.dumps({"type":"directive","local_id":8,"data":1})

MESSAGE_FAILURE = json.dumps({"type":"failure","data": [{"local_id":2,"error":1},{"local_id":1,"error":3}]})

#print "UDP target IP:", UDP_IP
#print "UDP target port:", UDP_PORT
#print "message:", MESSAGE_CONF
#print "message:", MESSAGE_MEASURE_1
#print "message:", MESSAGE_MEASURE_X
#print "message:", MESSAGE_DIRECTIVE
#print "message:", MESSAGE_FAILURE

#sock = socket.socket(socket.AF_INET6, # Internet
#					socket.SOCK_DGRAM) # UDP

#sock = socket.socket(socket.AF_INET, # Internet
#					socket.SOCK_DGRAM) # UDP

json_message = MESSAGE_CONF
#Envio paquete IP con JSON / SCAPY
IP_DEST = "fe90::0001" #tun0 iface
paquete = IPv6(src=IP_SRC, dst=IP_DEST) / UDP(dport=40000) / json_message
send(paquete)	

#sock.sendto(MESSAGE_CONF, (UDP_IP, UDP_PORT))
#sock.sendto(MESSAGE_MEASURE_1, (UDP_IP, UDP_PORT))
#sock.sendto(MESSAGE_MEASURE_X, (UDP_IP, UDP_PORT))
#sock.sendto(MESSAGE_MEASURE_1, (UDP_IP, UDP_PORT))
#sock.sendto(MESSAGE_FAILURE, (UDP_IP, UDP_PORT))
