#tun IP server
import multiprocessing
import binascii
import json
from pytun import TunTapDevice
import os
from nat_bridge_functions import *

tun = TunTapDevice(name='tun0')
tun.mtu = 1500

while True:
	buf = tun.read(tun.mtu)
	#p = multiprocessing.Process(target=data_processor.parse_message(addr, data))
	#p.start()
	packet = binascii.hexlify(buf).decode()
	
	dest_ip = get_dest_ip_from_scapy(packet)
	dest_udp_port = get_dest_udp_port_from_scapy(packet)

	#ethernet = packet[0:8]
	#ip_headers = packet[8:24]
	
	mes_type, local_id, data = get_json_content_from_scapy(packet)

	if int(dest_udp_port,16) == 40001:
		if (mes_type == 'directive'):
			print "TUN SERVER DIRECTIVE %s %s %s %s" % (dest_ip, mes_type, str(local_id), str(data))
			p = multiprocessing.Process(target=os.system("./nat_bridge_out.py %s %s %s %s" % (dest_ip, mes_type, str(local_id), str(data)) ))
			p.start()
		else:
			#polling configuration or failures
			print "TUN SERVER POLL CONF %s %s" % (dest_ip, mes_type)
			p = multiprocessing.Process(target=os.system("./nat_bridge_out.py %s %s" % (dest_ip, mes_type) ))
			p.start()



