from machine import UART,Pin
import utime,socket
import esp
nova={
'sleep':	([0xaa,0xb4,0x06,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0xff,0x05,0xab],[0xaa,0xc5,0x06,0x01,0x00,0x00,0xb4,0x6f,0x2a,0xab]),
'wakeup':	([0xaa,0xb4,0x06,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0xff,0x06,0xab],[0xaa,0xc5,0x06,0x01,0x01,0x00,0xb4,0x6f,0x2b,0xab]),
'passive':	([0xaa,0xb4,0x02,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0xff,0x02,0xab],[0xaa,0xc5,0x02,0x01,0x01,0x00,0xb4,0x6f,0x27,0xab]),
'active':	([0xaa,0xb4,0x02,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0xff,0x01,0xab],[0xaa,0xc5,0x02,0x01,0x00,0x00,0xb4,0x6f,0x26,0xab])
}

def get_data(cnt):
	pm25=pm10=i=0
	rsp=smp=b''
	#rst=[0x00,0x00,0x00,0x00]
#	urt.reset_input_buffer()
	while i<cnt:
		smp=urt.read(10)
		if smp:
			rsp+=smp
			conn.send('entering')
			while len(rsp)>=10:
				if rsp[0]==0xaa and rsp[1]==0xc0 and rsp[9]==0xab:
					if sum(rsp[2:8])%256==rsp[8]:
						pm25+=(rsp[3]<<8)+rsp[2]
						pm10+=(rsp[5]<<8)+rsp[4]
#						rst=[rst[j]+rsp[2+j] for j in range(4)]
						i+=1
					rsp=rsp[10:]
				else:
					rsp=rsp[1:]
	pm25=pm25//cnt
	pm10=pm10//cnt
	rst=[pm25%256,pm25>>8,pm10%256,pm10>>8]
#	rst=[int(rst[j]/cnt) for j in range(4)]

	tmp=[not rst[j] and 0xaa or 0xee for j in range(4)]
	rst=[not rst[j] and 0x0f or rst[j] for j in range(4)]
	return str(rst+tmp)

def send_cmd(cmd):
	i=0
	rsp=b''
#	urt.reset_input_buffer()
	urt.write(bytes(nova[cmd][0]))
	while i<5:
		smp=urt.read(10)
		if smp:
			conn.send('period......')
			rsp+=smp
			i+=1
			while len(rsp)>=10:
				if rsp[0]==0xaa and rsp[1]==0xc5 and rsp[9]==0xab and sum(rsp[2:8])%256==rsp[8]:
					if bytes(rsp[:10])==bytes(nova[cmd][1]):
						return True
					else:
						rsp=rsp[10:]
				else:
					rsp=rsp[1:]
	return False


#esp.osdebug(None)
urt=UART(0,9600)
urt.init(baudrate=9600,bits=8,parity=None,stop=1)
utime.sleep(3)

acpt=b''
wfs=Pin(2,Pin.OUT)
wfs.high()
skt=socket.socket(socket.AF_INET,socket.SOCK_STREAM)
skt.bind(('',80))
skt.listen(3)
conn,addr=skt.accept()
while 1:
	data=conn.recv(2)
	if not data: break
	acpt+=data
	wfs.low()
	if acpt[-2:].decode('uft-8')=='\r\n':
		acpt=acpt[:-2].decode('uft-8')
		if acpt=='1':
			send_cmd('wakeup')
#			rst=send_cmd('active')
#			conn.send(str(rst))
#			utime.sleep(10)
			conn.send(get_data(5))
			rst=send_cmd('sleep')
#			for j in range(5):
#				if rst: break
#				utime.sleep(1)
#				rst=send_cmd('sleep')
#			conn.send(str(rst))
		if acpt=='2':
			conn.send(urt.read(10))
			urt.write(bytes(nova['sleep'][0]))
		acpt=b''
	wfs.high()
urt.init(baudrate=115200,bits=8,parity=None,stop=1)
conn.close()
