#!/usr/bin/python3
import telnetlib,time
#srv='10.10.1.80'
srv="192.168.3.24"
port=80
cmdsrl={
	'Air':'1',
	'Start':'6',
	'Stop':'5',
	'Relay':'6'
}
tn=telnetlib.Telnet(srv,port,timeout=30)
time.sleep(0.5)


def rspd(rsp):
	rsp=[i for i in rsp]
#	print(rsp)
	if len(rsp)==9 and sum(rsp[:4])%256==rsp[8]:
		rsp=[rsp[i]&((rsp[4+i]-0xaa) and 0xff) for i in range(4)]
		return ((rsp[1]<<8)+rsp[0])/10,((rsp[3]<<8)+rsp[2])/10
	else:
		return 'Data serial error!!'


tn.write('1'.encode("ascii"))
#tn.write(bytes([2]))

#rsp=tn.read_until(bytes([13]))
#print(tn.read_some())
print(time.strftime("%Y-%m-%d %H:%M:%S", time.localtime()),rspd(tn.read_some()))
tn.close()

