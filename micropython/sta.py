import socket
from machine import Pin
skt=socket.socket(socket.AF_INET,socket.SOCK_STREAM)
skt.bind(('',80))
skt.listen(1)
conn,addr=skt.accept()
acpt=b''
wfs=Pin(2,Pin.OUT)
wfs.high()
while 1:
	data=conn.recv(2)
	if not data: break
	wfs.low()
	acpt+=data
	if acpt[-2:].decode('uft-8')=='\r\n':
		conn.sendall(acpt+'@')
		acpt=b''
	wfs.high()
conn.close()
