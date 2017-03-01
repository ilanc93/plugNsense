import mysql.connector
import json
import datetime
from directive_client_functions import *


def get_ip_from_node_ip(id_node):
	USER = "tesis";
	PASS = "tesis";
	HOST = "localhost";
	DB =  "tesis";
	
	cnx = mysql.connector.connect(user=USER, password=PASS, host=HOST, database=DB)
	cursor = cnx.cursor()

	query = ("SELECT node_ip FROM tesis.node WHERE id_node = %d") % ( id_node );
	cursor.execute(query);

	for node_ip in cursor:
        	IP = "%s" % node_ip
		#print node_ip
	cnx.commit()
	cursor.close()
	cnx.close()
	
	return IP


def save_directive(id_node, id_local, directive):	
	USER = "tesis";
	PASS = "tesis";
	HOST = "localhost";
	DB =  "tesis";
	
	cnx = mysql.connector.connect(user=USER, password=PASS, host=HOST, database=DB)
	cursor = cnx.cursor()

	query = ("SELECT id_node_device FROM tesis.node_local_device WHERE id_node = %d AND id_local = %d ORDER BY id_node_device DESC LIMIT 1") % ( id_node, id_local );
	cursor.execute(query);
	#print cursor
	#for id_node_device in cursor:
        	#node_device = id_node_device
	node_device = int(cursor.fetchone()[0]) 	
	#print node_device
	cursor.execute(("INSERT INTO tesis.directive VALUES (NULL, %d, %d, now())") % ( node_device, directive ))
	cnx.commit()
	cursor.close()
	cnx.close()


def directive_prepare_json(id_local, directive):
	#{"type":"directive","local_id":#,"data": [#1]}
	data = {}
	data['type'] = 'directive'
	data['local_id'] = id_local
	data['data'] = directive
	
	json_message = json.dumps(data)

	return json_message

def action_prepare_json(action):
	#{"type":"xxxx"}
	data = {}
	if ( action == 6 ):
		mes_type = 'polling_configuration'
	else:
		mes_type = 'polling_failure'
	data['type'] = mes_type
	
	json_message = json.dumps(data)

	return json_message
