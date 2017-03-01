import mysql.connector

def save_measurements(address, message):
	IP = address	
	id_local = message['local_id']

	USER = "tesis";
	PASS = "tesis";
	HOST = "localhost";
	DB =  "tesis";
	cnx = mysql.connector.connect(user=USER, password=PASS, host=HOST, database=DB)
	cursor = cnx.cursor()

	id_node_device = int(get_id_node_device(cursor, IP, id_local))
	
	insert_measurements(message, id_node_device, cursor)
		
	cnx.commit()
	cursor.close()
	cnx.close()




def get_id_node_device(cursor, IP, id_local):
	query = ("SELECT nld.id_node_device FROM tesis.node n INNER JOIN tesis.node_local_device nld ON n.id_node = nld.id_node WHERE node_ip = '%s'") % (IP);
	cursor.execute(query);
	for id_node_device in cursor:
        	id_node_device = "%d" % id_node_device	
	return id_node_device
	

def insert_measurements(message, id_node_device, cursor):
	print (message)
	if len( message['data'] ) == 1:
		measure = message['data'][0]
		#print (('%s') % str(measure) )
		cursor.execute(("INSERT INTO tesis.measure VALUES (NULL, %d, now(), %d, DEFAULT)") % (measure, id_node_device))
	else:
		for x in range(0, len(message['data']) ):
			measure = message['data'][x]
			#print (measure)
			timestamp = message['timestamp'][x]
			cursor.execute(("INSERT INTO tesis.measure VALUES (NULL, %d, '%s', %d, DEFAULT)") % (measure, timestamp, id_node_device))


	
