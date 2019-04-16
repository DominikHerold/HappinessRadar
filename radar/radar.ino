#include <ESP8266WiFi.h>

unsigned long act_milli;

/*****************************************************************
/* Debug output                                                  *
/*****************************************************************/
void debug_out(const String& text, const bool linebreak) {
	if (linebreak) {
		Serial.println(text);
	} else {
		Serial.print(text);
	}
}

int wasRed = 0;
int wasGreen = 0;
unsigned long last_pushed;

const unsigned long pause_between_update_attempts = 86400000;

/*****************************************************************
/* Configuration part start                                      *
/*****************************************************************/
const char* ssid = "foo";
const char* password = "foo";
const char* host = "192.168.42.42";
int port = 5000;
String location = "leipzig";
/*****************************************************************
/* Configuration part end                                        *
/*****************************************************************/

void sendData(const String& color){
	WiFiClient client;
	
	Serial.printf("\n[Connecting to %s ... ", host);
	if (client.connect(host, port)){
		Serial.println("connected]");
		
		Serial.println("[Sending a request]");
		
		client.print(String("GET /api/happiness?location=" + location + "&color=" + color) + " HTTP/1.1\r\n" +
                 "Host: " + host + ":" + port + "\r\n" +
                 "Connection: close\r\n" +
                 "\r\n"
                );
				
		Serial.println("[Response:]");
		while (client.connected() || client.available()){
			if (client.available()){
				String line = client.readStringUntil('\n');
				Serial.println(line);
			}
		}
		client.stop();
		Serial.println("\n[Disconnected]");
	}
	else{
		Serial.println("connection failed!]");
		client.stop();
	}
}

void setup() 
{
  Serial.begin(9600);
  
  pinMode(D7, INPUT_PULLUP); 
  pinMode(D3, INPUT_PULLUP); 
  
  last_pushed = millis();
  
  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
	  delay(500);
	  Serial.print(".");
  }
  
  Serial.println(" connected");
}

void loop() 
{  
  act_milli = millis();
  
  if (digitalRead(D7) == HIGH){
	 wasGreen = 0;
  }
  else if (wasGreen == 0 && (act_milli - last_pushed) > 1000){
	  debug_out(F("GREEN\n"),1);
	  sendData("green");
	  wasGreen = 1;
	  last_pushed = millis();
  }
  
  if (digitalRead(D3) == HIGH){
	 wasRed = 0;
  }
  else if (wasRed == 0 && (act_milli - last_pushed) > 1000){
	  debug_out(F("RED\n"),1);
	  sendData("red");
	  wasRed = 1;
	  last_pushed = millis();
  }
  
  if (act_milli > (2 * pause_between_update_attempts)) {
	ESP.restart();
  }
  
  if (WiFi.status() != WL_CONNECTED) {  // reconnect if connection lost
	int retry_count = 0;
	debug_out(F("Connection lost, reconnecting "),0);
	WiFi.reconnect();
	while ((WiFi.status() != WL_CONNECTED) && (retry_count < 20)) {
		delay(500);
		debug_out(".",0);
		retry_count++;
	}
	
	debug_out("",1);
  }
}
