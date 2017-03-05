#include <ESP8266WiFi.h>
#include <ESP8266WiFiScan.h>
#include <PMs.h>

#define idc D0
#define relay D4
const char* ssid="eel";
const char* pswd="102672102672";
WiFiServer srv(80);
WiFiClient clt;
PMs pms(14,12);


void setup() {
	Serial.begin(9600);
//	pms.begin();
	pinMode(idc,OUTPUT);
	pinMode(relay,OUTPUT);
	digitalWrite(relay,1);
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
	String slt=clt.readStringUntil('\n');
	slt.trim();
	byte cmd=atoi(slt.c_str());
//	int cmd=clt.read();
	digitalWrite(idc,0);
	switch(cmd) {
	case 0 ... 4:
		clt.println(pms.writeOrder(cmd)*100);
		break;
	case 5:
		mesr(20,5,false,0);
		break;
	case 6:
		mesr(10,5,true,5);
		break;
	case 7:
		if (digitalRead(relay)==1) digitalWrite(relay,0);
		break;
	case 8:
		if (digitalRead(relay)==0) digitalWrite(relay,1);
		break;
	default:
		delay(300);
	}
	digitalWrite(idc,1);
}

void mesr(int mxt, int cnt, bool rqst, int itv) {
	byte rst[4];
	pms.measure(mxt,cnt,rqst,itv,rst);
	for (byte i=0;i<4;i++) Serial.println(rst[i]);
//	Serial.println();
//	Serial.println((char*)rst,4);
}

void pickup() {
	ESP8266WiFiScanClass scan;
	const char* ssid[]={"eel","bjcj-wifi1","bjcj-wifi5"};
	const char* pswd[]={"102672102672","bjcjy123456","bjcj8888"};
	int apn=scan.scanNetworks();
	bool ap=false;
	for(int i=0;i<apn;i++) {
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
