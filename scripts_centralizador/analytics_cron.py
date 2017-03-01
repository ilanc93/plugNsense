#!/usr/bin/python


from time import sleep
import os
import mysql.connector
from analytics_functions import * 

#unique database connection for cronjob
USER = "tesis";
PASS = "tesis";
HOST = "localhost";
DB =  "analytics";
cnx = mysql.connector.connect(user=USER, password=PASS, host=HOST, database=DB)
cursor = cnx.cursor()


routine_calculate_analytics(cursor)


cnx.commit()
cursor.close()
cnx.close()

