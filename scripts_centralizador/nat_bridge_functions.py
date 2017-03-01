import mysql.connector
import json
import datetime
#from datetime import datetime
#import datetime

def parse_message_type(payload_string):
	return payload_string[0] + payload_string[1]


def parse_payload_measurements(payload_string):

	'''
	private_address "0x0E"
	#### Envio de datos ####
	#TYPE = 01 , TIME = 031B, LOCAL_ID = 01, DATA = 08 (MEAS_AMNT) + 0174 5283 1111 03AC E4F2 04B1 94A1 8511 (MEAS)
	PAQUETE: "01031B010801745283111103ACE4F204B194A18511"

	private_address "0x1B"
	#### Envio de datos ####
	#TYPE = 01 , TIME = 021B, LOCAL_ID = 00, DATA = 06 (MEAS_AMNT) + 2536 1111 E4F2 114C 94A1 8511 (MEAS)
	PAQUETE: "01021B000625361111E4F2114C94A18511"

	
	'''

	timestamp = payload_string[2] + payload_string[3] + payload_string[4] + payload_string[5]
	local_id = payload_string[6] + payload_string[7]
	meas_amnt = payload_string[8] + payload_string[9]
	meas = []
	value = ''
	
	for x in range(0, int(meas_amnt) ):
		for y in range(0,4):
			value += payload_string[10 + x*4 + y]
		meas.append( int( map(ord, value)[0] ) )
		value = ''
	
	return timestamp, local_id, meas_amnt, meas


def get_ip_and_time_info(local_id, private_address):
	USER = "tesis";
	PASS = "tesis";
	HOST = "localhost";
	DB =  "tesis";
	
	cnx = mysql.connector.connect(user=USER, password=PASS, host=HOST, database=DB)
	cursor = cnx.cursor()

	query = ("SELECT node_ip, sampling_period, timestamp_sent FROM tesis.node no INNER JOIN tesis.nat na ON no.id_node = na.id_node INNER JOIN tesis.network ne ON ne.id_network = na.id_network INNER JOIN tesis.clk_reset c ON c.id_network = ne.id_network INNER JOIN tesis.node_local_device nld ON nld.id_node = no.id_node INNER JOIN tesis.io_type io ON io.id_io = nld.id_io WHERE nld.id_local = '%s' AND na.private_address = '%s'") % (local_id, private_address);
	cursor.execute(query);

	for node_ip, sampling_period, timestamp_sent in cursor:
		IP = "%s" % node_ip
		sampling = "%d" % sampling_period
		clk_rst = "%s" % timestamp_sent	
	
	cnx.commit()
	cursor.close()
	cnx.close()
	
	return IP, sampling, clk_rst

def prepare_timestamp_array(clk_rst, sampling_period, timestamp, qtty):
	millis = int( map(ord, timestamp)[0] )
	secs = int( round( millis / 1000 ) );
	
	clk_rst2 = datetime.datetime.strptime(clk_rst, '%Y-%m-%d %H:%M:%S')
	ts = clk_rst2 + datetime.timedelta(seconds=secs)
	initial_timestamp = ts	

	timestamp_array = []
	time = initial_timestamp
	timestamp_array.append(time.strftime('%Y-%m-%d %H:%M:%S'))
	for x in range( 1, int(qtty) ):
		time += datetime.timedelta(seconds= int(sampling_period))		
		timestamp_array.append(time.strftime('%Y-%m-%d %H:%M:%S'))
	
	return timestamp_array


def build_json_measurements(local_id, timestamp_array, measurements_array, qtty):
	data = {}
	data['type'] = 'measurements'
	data['local_id'] = local_id
	time = []
	meas = []
	for  x in range( 0, int(qtty) ):
		time.append( timestamp_array[x] )
		meas.append( measurements_array[x] )
				
	data['timestamp'] = time
	data['data'] = meas
	json_message = json.dumps(data)

	return json_message



def parse_payload_io(payload_string):

	'''
	#Envio configuracion en reconocimiento
	#TYPE = 07, DATA = 05 (IO_QTTY) + 0000 (LOCAL_ID 0, TIPO 1) 0100 (LOCAL_ID 1, TIPO 3) + 0201 (LOCAL_ID 2, TIPO 1) + 0302 (LOCAL_ID 3, TIPO 2) + 0401(LOCAL_ID 4, TIPO 1)
	PAQUETE: "070500010103020103020401"
	'''

	io_qtty = int( payload_string[2] + payload_string[3] )
	
	io_configurations = []
	IO = ''
	LID = ''
	for x in range(0, io_qtty ):
		LID = payload_string[ 4 + 4*x ] + payload_string[ 4 + 4*x + 1 ]
		IO = payload_string[ 4 + 4*x + 2 ] + payload_string[ 4 + 4*x + 3 ]		
		io_configurations.append( {'local_id': int( LID ), 'io_type': int( IO )} )
	
	return io_configurations

def get_ip_from_private_address(private_address):
	USER = "tesis";
	PASS = "tesis";
	HOST = "localhost";
	DB =  "tesis";
	
	cnx = mysql.connector.connect(user=USER, password=PASS, host=HOST, database=DB)
	cursor = cnx.cursor()

	query = ("SELECT node_ip FROM tesis.node no INNER JOIN tesis.nat na ON no.id_node = na.id_node WHERE na.private_address = '%s'") % ( private_address );
	cursor.execute(query);

	for node_ip in cursor:
        	IP = "%s" % node_ip
		
	cnx.commit()
	cursor.close()
	cnx.close()
	
	return IP

def get_private_address_from_ip( ip ):
	USER = "tesis";
	PASS = "tesis";
	HOST = "localhost";
	DB =  "tesis";
	
	cnx = mysql.connector.connect(user=USER, password=PASS, host=HOST, database=DB)
	cursor = cnx.cursor()

	query = ("SELECT private_address FROM tesis.node no INNER JOIN tesis.nat na ON no.id_node = na.id_node WHERE no.node_ip = '%s'") % ( ip );
	cursor.execute(query);

	for private_address in cursor:
        	addr = private_address[0]
		
	cnx.commit()
	cursor.close()
	cnx.close()
	
	return addr

def build_json_configuration( conf_data ):
	#{"type":"configuration","data": [{"local_id":2,"io_type":1},{"local_id":1,"io_type":2},{"local_id":1,"io_type":3}]}	
	data = {}
	data['type'] = 'configuration'
	data['data'] = conf_data
	json_message = json.dumps(data)

	return json_message
	
def get_dest_ip_from_scapy( packet ):
	#source_ip = ':'.join( (packet[24:28],packet[28:32],packet[32:36],packet[36:40],packet[40:44],packet[44:48],packet[48:52],packet[52:56]) )	
	dest_ip = ':'.join( (packet[56:60],packet[60:64],packet[64:68],packet[68:72],packet[72:76],packet[76:80],packet[80:84],packet[84:88]) )
	
	return dest_ip

def get_dest_udp_port_from_scapy( packet ):
	return packet[92:96]

def get_json_content_from_scapy(packet):
	message = packet[104:len(packet)]
	mes = message.decode("hex")
	json_decoded = json.loads(mes)
	if ( len(json_decoded)==1 ):
		mes_type = json_decoded['type'] #polling_configuration  or polling_failure  
		data = ''
		local_id = ''
	else:
		mes_type = json_decoded['type']
		data = json_decoded['data'] #directive
		local_id = json_decoded['local_id'] #device
	
	return mes_type, local_id, data	
