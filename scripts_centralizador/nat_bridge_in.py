#!/usr/bin/python

# execute with ./nat_bridge_in.py 0E 01031B010801745283111103ACE4F204B194A18511
# execute with ./nat_bridge_in.py 1B 01021B000625361111E4F2114C94A18511
## execute with ./nat_bridge_in.py 37 070500010103020103020401
import sys
from nat_bridge_functions import *
from scapy.all import *
 
# Get the total number of args passed
total = len(sys.argv)
# Get the arguments list 
cmdargs = str(sys.argv)
 
#expected private_address first, then payload string
private_address = int( str(sys.argv[1]), 16 ) #converted to decimal
payload_string = str(sys.argv[2])

message_type = parse_message_type(payload_string)

if message_type == "01": #receiving measurements
	#parse string and get local timestamp offset, local_id, measurements amount, measurements array (decimal int)
	timestamp, local_id, meas_amnt, meas = parse_payload_measurements(payload_string)
	#get node ip, sensor sampling period, clock reset timestamp
	IP_SRC, sampling_period, clk_rst = get_ip_and_time_info(local_id, private_address)
	#build timestamp array (datetime format) from clock reset timestamp, local timestamp offset, and sensor sampling period
	timestamp_array = prepare_timestamp_array(clk_rst, sampling_period, timestamp, meas_amnt)
	#build message and json encode
	json_message = build_json_measurements(local_id, timestamp_array, meas, meas_amnt)
	
elif message_type == "07": #receiving configuration
	#parse string and get pairs of local_id and io_type	
	io_conf = parse_payload_io(payload_string)
	#get IP	
	IP_SRC = get_ip_from_private_address(private_address)
	#build message and json encode		
	json_message = build_json_configuration( io_conf )
	
else:
	"Unknown message"
	#WHEN DISCOVERY INSERT INTO NAT AND NODE

#Envio paquete IP con JSON / SCAPY
IP_DEST = "fe90::0001" #tun0 iface
IP_SRC = IP_SRC
paquete = IPv6(src=IP_SRC, dst=IP_DEST) / UDP(dport=40000) / json_message
send(paquete)	


