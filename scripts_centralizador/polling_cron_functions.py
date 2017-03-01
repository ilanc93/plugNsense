from time import sleep
import os
import mysql.connector


def routine_poll_measurements(cursor):
	#Rutina de polling de datos
	cursor_aux = get_measurements_polling_st7540(cursor)
	DATA = 2 #poll measurements
	for id_node, id_local in cursor_aux:
		node = "%d" % id_node
		LOCALID = "%d" % id_local
		os.system("./directive_api_client.py %s %s %s" % ( str(node), str(LOCALID), str(DATA) ) )
		sleep(1)

def routine_clk_reset(cursor):
	TYPE_MESSAGE = 5
	os.system("./directive_api_client.py %s" % ( str(TYPE_MESSAGE) ) )
	update_last_broadcast(cursor)
	sleep(1)

'''	
#Rutina de polling de fallas
	for (A)
		TYPE_MESSAGE = 8
		id_node polling fallas
		delay ms
'''

def routine_poll_configuration(cursor):
	#Rutina de polling de configuracion
	TYPE_MESSAGE = 6
	cursor_aux = get_configuration_polling_st7540(cursor)
	for id_node in cursor_aux:
		node = "%d" % id_node
		print "NODE TO CONF %s" % str(node)
		os.system("./directive_api_client.py %s %s" % ( str(node), str(TYPE_MESSAGE) ) )
		sleep(1)

def routine_discovery():
	#Rutina de reconocimiento de nuevos nodos
	TYPE_MESSAGE = 0
	for x in range(3):
		os.system("./directive_api_client.py %s" % ( str(TYPE_MESSAGE) ) )
		sleep(1)

def get_measurements_polling_st7540(cursor):
	#(B) Agarrar todos los id_node, id_node_device de los sensores que hay que pollear de la red st7540
	query = ("SELECT no.id_node, nld.id_local FROM tesis.node no INNER JOIN tesis.node_local_device nld ON no.id_node = nld.id_node INNER JOIN tesis.io_type io ON io.id_io = nld.id_io WHERE no.modem = 'ST7540' AND io.io_type = 'I'");
	cursor.execute(query);
	return cursor

def get_configuration_polling_st7540(cursor):
	#(C) Agarrar todos los id_node de los nodos de la red st7450 que no tienen ningun id_node_device
	query = ("SELECT no.id_node FROM tesis.node no LEFT JOIN tesis.node_local_device nld ON no.id_node = nld.id_node WHERE nld.id_node IS NULL");
	cursor.execute(query)
	return cursor

def update_last_broadcast(cursor):
	query = ("UPDATE tesis.clk_reset SET timestamp_sent = now() WHERE id_network = (SELECT id_network FROM tesis.network WHERE modem = 'ST7540')");
	cursor.execute(query)
