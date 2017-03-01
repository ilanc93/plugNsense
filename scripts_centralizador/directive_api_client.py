#!/usr/bin/python


#Recieves 1, 2 or 3 arguments apart from script name/path
	#If 1, then node discovery or Clock reset
		#0 for dicovery
		#5 for clock reset
	#If 2, then configuration polling or failure polling
		###destination id_node, type of polling
		#6 polling conf
		#7 polling fallas
	#If 3, then directive (data polling or action requiered)
		# execute with ./directive_api_client.py 1 2 1
		###destination id_node, id_local, action/directive
		#{"type":"directive","local_id":#,"data": #}

import sys
from scapy.all import *
from directive_client_functions import *
 
# Get the total number of args passed
total_args = len(sys.argv)

#####BUILD UDP SERVER PORT 40001
##################################
if ( total_args==2 ):
	#CLK RESET / DISCOVERY	
	action = int( sys.argv[1] )
	id_node = 0 
	if ( action == 5 ):
		#CLK RST
		message = "CLOCKRESET"
	elif ( action == 0 ):
		#DISCOVERY
		message = "NODEDISCOVERY"
	else:
		print ("Argument error")

elif ( total_args==3 ):
	#CONF POLLING / FAILURE POLLING
	id_node = int( sys.argv[1] )
	action = int( sys.argv[2] )
	print action 
	if ( (action == 6) | (action == 7) ):
		#print "NODE TO CONFIGURE %d ACTION %d" % (id_node, action)
		#POLLING CONF OR FAILURE POLLING
		message = action_prepare_json(action)
	else:
		print ("Argument error")

elif ( total_args==4 ):
	#action required / measurements polling
	id_node = int( sys.argv[1] ) 
	id_local = int( str(sys.argv[2]) )
	directive = int( str(sys.argv[3]) )

	save_directive(id_node, id_local, directive)
	message = directive_prepare_json(id_local, directive)
	
else:
	print ("TYPE NOT CONFIGURED")

 
if (id_node != 0):
	IP_DEST = get_ip_from_node_ip(id_node) #linux kernel must route to the correct interface
else:
	IP_DEST = 'fe90::1'


#Envio paquete IP con JSON / SCAPY
IP_SRC = '::'
paquete = IPv6(src=IP_SRC, dst=IP_DEST) / UDP(dport=40001) / message
send(paquete)	
print message
#print paquete

