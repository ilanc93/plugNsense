import mysql.connector

def save_configuration(address, message_decoded):

	#{"type":"configuration","data": [{"local_id":2,"io_type":1},{"local_id":1,"io_type":2},{"local_id":1,"io_type":3}]}

	IP = address	
	to_configure = message_decoded['data']
	print (to_configure)
	USER = "tesis";
	PASS = "tesis";
	HOST = "localhost";
	DB =  "tesis";
	cnx = mysql.connector.connect(user=USER, password=PASS, host=HOST, database=DB)
	cursor = cnx.cursor()
	print (IP)
	query = ("SELECT n.id_node FROM tesis.node n WHERE n.node_ip = '%s'") % ( IP );
	cursor.execute(query)
	rows = cursor.fetchall()
	if ( not rows ):
		print ("AAAAAAAAA")
		print (str(IP)[0:4])
		if ( ((str(IP)[0:4]) == 'fe80') ):
			query = ("INSERT INTO tesis.node VALUES (NULL, '%s', 'QCA7000', 1, NULL, 2)") % ( IP );
			cursor.execute(query);
			query = ("SELECT n.id_node FROM tesis.node n WHERE n.node_ip = '%s'") % ( IP );
			cursor.execute(query);
			rows = cursor.fetchall()

	for id_node in rows:
		id_node = int( "%d" % id_node )
	for x in range( 0,len(to_configure) ):
		id_local = int( to_configure[x]['local_id'] )
		IO = int( to_configure[x]['io_type'] )	
		print (IO)	
		
		query = ("SELECT io.id_io FROM tesis.io_type io WHERE io.io_number = %d") % ( IO );
		cursor.execute(query);
		for id_io in cursor:
        		id_io = int( "%d" % id_io )
		cursor.execute( ("INSERT INTO tesis.node_local_device VALUES (NULL, %d, %d, %d, '', DEFAULT)") % (id_node, id_local, id_io) );		
		
	cnx.commit()
	cursor.close()
	cnx.close()



