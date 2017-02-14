# This file is executed on every boot (including wake-boot from deepsleep)
#import esp
#esp.osdebug(0)
import gc
#import webrepl
#webrepl.start()
gc.collect()

def did(fn):
	exec(open(fn+'.py','rb').read())

def wrs():
	import webrepl
	webrepl.start()

#import utime
#import os
#os.remove('main.py')
#utime.sleep(5)
