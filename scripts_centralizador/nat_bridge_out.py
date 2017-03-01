#!/usr/bin/python3

from nat_bridge_functions import get_private_address_from_ip
#import constantes
import multiprocessing
import binascii
import json
import ipaddress
import sys


SOF_BYTE = 171
ORIG = 0
PAYDATA = []


# Get the total number of args passed
total_args = len(sys.argv)
print (total_args)

if ( total_args == 2 ):
	DEST = 255
	mes_type = str(sys.argv[1])
	if ( mes_type == "CLOCKRESET" ):
		TYPE = 5
		PAYDATA.append(TYPE)
	elif ( mes_type == "NODEDISCOVERY" ):
		print ("NODE DISCOVERY")
	else:
		print ("Argument error")

elif ( (total_args == 3) | (total_args == 5) ):
	ip_dir = sys.argv[1] 
	mes_type = str( sys.argv[2] )

	if ( mes_type == "directive" ):
		TYPE = 2
	else: #configuration polling
		TYPE = 6

	PAYDATA.append(TYPE)
	dest_ip = ipaddress.IPv6Address(ip_dir)
	private_address = get_private_address_from_ip( dest_ip )
	DEST = private_address

	if ( total_args == 5 ):
		#Add local_id and data for action/directives
		#DIRECTIVE
		LOCAL_ID = int( str(sys.argv[3]) )
		DATA = int( str(sys.argv[4]) )

		PAYDATA.append(LOCAL_ID)
		PAYDATA.append(DATA)
else:
	print ("Argument error")


#####https://wiki.python.org/moin/BitManipulation

PAYLEN = len(PAYDATA) #each is 1 byte -> len = #bytes

frame_array = []
frame_array.append(SOF_BYTE)
frame_array.append(SOF_BYTE)
frame_array.append(SOF_BYTE)
frame_array.append(SOF_BYTE)
frame_array.append(DEST)
frame_array.append(ORIG)
frame_array.append(PAYLEN)
for x in range((len(PAYDATA))):
	frame_array.append(PAYDATA[x])

CHECKSUM = 0
for x in range(len(frame_array)):
    CHECKSUM += bin(x).count('1')

frame_array.append(CHECKSUM)
print (frame_array)

'''
string_array = ''.join('{:02x}'.format(x) for x in frame_array)
print (string_array)

bin_string = bin(int(string_array, 16))[2:]
print (bin_string)
'''
