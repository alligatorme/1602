#include "PMs.h"

PMs::PMs(int rcv, int trm ){
//	SoftwareSerial _swSr(rcv,trm,false,256);
	_swSr=new SoftwareSerial(rcv,trm,false,256);
}
void PMs::begin(){
	_swSr->begin(9600);
}

void PMs::measure(byte mxt, byte cnt, bool rqst, byte itv, byte* rst) {
	pms[1]=0;
	pms[0]=0;
	byte i=0;
	writeOrder(2);
	if (rqst) writeOrder(3);
	delay(1000*mxt);
	cleanBuffer();
	while (i<cnt) {
		if (rqst) {
			for(byte j=0;j<5;j++) {
				if (writeOrder(4)==1) {i++; break;}
			}
			delay(1000*itv);
		}
		else {
			if (readData()==1) i++;
		}
	}
	pms[0]/=i;pms[1]/=i;
	rst[0]=pms[0]&255;rst[1]=pms[0]>>8;rst[2]=pms[1]&255;rst[3]=pms[1]>>8;
	writeOrder(0);
}

byte PMs::readData() {
	if (_swSr->available()>0 && _swSr->read()==0xaa)
	{
		byte pm[9];
		_swSr->readBytes(pm,9);
		byte tmp=pm[1]+pm[2]+pm[3]+pm[4]+pm[5]+pm[6];
		if (pm[8]==0xab && (tmp&255)==pm[7])
		{
			if (pm[0]==0xc0) {
				pms[0]+=(pm[2]<<8)|pm[1];
				pms[1]+=(pm[4]<<8)|pm[3];
				return 1;
			}
			if (pm[0]==0xc5) {
				for(byte i=0;i<4;i++) {
					if (!memcmp(_rsp[i],pm+1,4)) return i+2;
				}
			}
		}
		delay(200);
	}
	return 0;
}

byte PMs::writeOrder(byte id) {
	cleanBuffer();
	_swSr->write(_cmd[id],19);
	for(byte i=0;i<5;i++) {
		delay(300);
		byte chk=readData();
		if (chk>0) return chk;
	}
	return 0;
}

void PMs::cleanBuffer() {
	while(_swSr->available()) _swSr->read();
}

