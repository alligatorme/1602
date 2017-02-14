#include <ESP8266WiFi.h>
#include <ESP8266WiFiScan.h>

#define idc D0
int pm10,pm25;
const char* ssid="eel";
const char* pswd="102672102672";
WiFiServer srv(80);
WiFiClient clt;

void setup() {
	Serial.begin(9600);
	pinMode(idc,OUTPUT);
	digitalWrite(idc,0);
	pickup();
//	WiFi.begin(ssid,pswd);
	while(WiFi.status()!=WL_CONNECTED) delay(1000);
  Serial.println(WiFi.localIP());
	digitalWrite(idc,1);
	srv.begin();
	clt=srv.available();
}

void loop() {
	if (!clt) {clt=srv.available(); return;}

	while(!clt.available()){
		if(!clt.connected()) {clt=srv.available(); return;}
		delay(100);
	}
	String slt=clt.readStringUntil('\r\n');
	slt.trim();
	int cmd=atoi(slt.c_str());
//	int cmd=clt.read();
	digitalWrite(idc,0);
	switch(cmd) {
	case 1:
		mesr(10,5,false);
		break;
	case 2 ... 5:
		send_cmd(cmd-2);
		break;
	default:
		delay(300);
	}
	digitalWrite(idc,1);
}

void mesr(int itv, int cnt, bool rqst) {
	pm10=0;
	pm25=0;
	int i=0;
	send_cmd(1);
	if (rqst) send_cmd(3);
	delay(1000*itv);
	rmbuf();
	while (i<=cnt) {
		if (rqst) {
			send_cmd(5);
			for(int j=0;j<5;j++) {
				if (get_pm()==1) {i++; break;}
			}
			delay(1000*2);
		}
		else {
			if (get_pm()==1) i++;
		}
	}
	pm25/=i;pm10/=i;
	byte rst[9];
	rst[0]=pm25%256;rst[1]=pm25>>8;rst[2]=pm10%256;rst[3]=pm10>>8;
	int sha=0;
	for(int i=0;i<4;i++) {
		sha+=rst[i];
		rst[i+4]=!rst[i]?0xaa:0xee;
		rst[i]=!rst[i]?0x0f:rst[i];
	}
	rst[8]=sha%256;
	clt.write((char*)rst,9);
	send_cmd(0);
}

int get_pm() {
	if (Serial.available()>0 && Serial.read()==0xaa)
	{
		byte pm[9];
		Serial.readBytes(pm,9);
		int tmp=pm[1]+pm[2]+pm[3]+pm[4]+pm[5]+pm[6];
		if (pm[8]==0xab && (tmp%256)==pm[7])
		{
			if (pm[0]==0xc0) {
				pm25+=(pm[2]<<8)+pm[1];
				pm10+=(pm[4]<<8)+pm[3];
				return 1;
			}
			if (pm[0]==0xc5) {
				byte rspd[4][4]={{0x06,0x01,0x00,0x00},{0x06,0x01,0x01,0x00},{0x02,0x01,0x00,0x00},{0x02,0x01,0x01,0x00}};
				for(int i=0;i<4;i++) {
					if (!memcmp(rspd[i],pm+1,4)) return i+2;
//					bool chk=false;
//					for(int j=0;j<4;j++) {
//						chk=(rspd[i][j]^pm[j+1])||chk;
//					}
//					if (!chk) return i+2;
				}
			}
		}
		delay(200);
	}
	return 0;
}

int send_cmd(int id) {
	byte cmdset[5][19]={{0xaa,0xb4,0x06,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0xff,0x05,0xab},\
						{0xaa,0xb4,0x06,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0xff,0x06,0xab},\
						{0xaa,0xb4,0x02,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0xff,0x01,0xab},\
						{0xaa,0xb4,0x02,0x01,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0xff,0x02,0xab},\
						{0xaa,0xb4,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0xff,0x02,0xab}};
	rmbuf();
	Serial.write(cmdset[id],19);
	for(int i=0;i<5;i++) {
		delay(300);
		int chk=get_pm();
		if (chk>1) {clt.println(chk*100); break;}
	}
}

void rmbuf() {
	while(Serial.available()) Serial.read();
}

void pickup() {
	ESP8266WiFiScanClass scan;
	char* ssid[]={"eel","bjcj-wifi1","bjcj-wifi5"};
	char* pswd[]={"102672102672","bjcjy123456","bjcj8888"};
	int apn=scan.scanNetworks();
	bool ap=false;
	for(int i=0;i<apn;i++) {
//		Serial.println(scan.SSID(i));
		for(int j=0;j<3;j++){
			if (scan.SSID(i)==ssid[j]) {
				WiFi.begin(ssid[j],pswd[j]);
				ap=true;
				break;
			}
		 if (ap) break;
		}
	}
}
