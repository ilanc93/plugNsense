#!/usr/bin/python

###sudo chmod +x /home/USUARIO/TESIS/polling_cron.py
###crontab -e
###* * * * * /home/USUARIO/TESIS/polling_cron.py
from polling_cron_functions import *
import mysql.connector

#unique database connection for cronjob
USER = "tesis";
PASS = "tesis";
HOST = "localhost";
DB =  "tesis";
cnx = mysql.connector.connect(user=USER, password=PASS, host=HOST, database=DB)
cursor = cnx.cursor()

print "##############routine_poll_measurements##########"
routine_poll_measurements(cursor)

print "##############routine_clk_reset###############"
routine_clk_reset(cursor)

print "#############routine_poll_conf###############"
routine_poll_configuration(cursor)

print "###############routine_discovery###############"
routine_discovery()

cnx.commit()
cursor.close()
cnx.close()

