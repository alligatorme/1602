import network,utime
sta_if = network.WLAN(network.STA_IF)
sta_if.active(True)
aps=[i[0].decode('utf-8') for i in sta_if.scan()]
print(aps)
aplist={'eel':'102672102672','bjcj-wifi5':'bjcj8888','bjcj-wifi1':'bjcjy123456'}
for k,v in aplist.items():
	if k in aps:
		sta_if.connect(k,v)
		break
utime.sleep(5)
if sta_if.isconnected():
	print(sta_if.ifconfig())
else:
	print('No ap is avaliable!')
