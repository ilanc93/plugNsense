import multiprocessing
import json
from db_insert_measurements import *
from db_insert_configuration import *

def parse_message(address, json_message):
	
	message_decoded = json.loads( json_message.decode('utf-8') )
	
	if ( message_decoded['type']=='configuration' ):
		save_configuration(address, message_decoded)
	elif ( message_decoded['type']=='measurements' ):
		save_measurements(address, message_decoded)
	elif message_decoded['type']=='directive':
		pass	
	elif message_decoded['type']=='failure':
		pass	
	else:
		print ("TYPE MESSAGE NO CONFIGURADO")


