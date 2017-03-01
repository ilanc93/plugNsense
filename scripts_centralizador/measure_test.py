import mysql.connector
import random
import time

USER = "tesis";
PASS = "tesis";
HOST = "localhost";
DB =  "tesis";
cnx = mysql.connector.connect(user=USER, password=PASS, host=HOST, database=DB)
cursor = cnx.cursor()

ran = 0
for x in range( 0, 20 ):
	ran = random.random()
	query = ("INSERT INTO tesis.measure VALUES (NULL, %d, now(),,NULL) ") % ( ran*1024 );
	cursor.execute(query)
	time.sleep(4)
