// Tawan Hohum :)
// Internship student
// Cyber-Physical System, NECTEC NSTDA
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define PIR 15
#define relay 4

const char* ssid = "Your SSID";
const char* password = "Your WiFi Password";
const char* mqtt_server = "broker.netpie.io";
const int mqtt_port = 1883;
const char* mqtt_Client = "Your Client ID";
const char* mqtt_username = "Your Token";
const char* mqtt_password = "Your Secret";

char msg[50];
int lighting = 0;
int PIRstate = 0;
int period = 1500;
unsigned long countTime = 0;

WiFiClient espClient;
PubSubClient client(espClient);

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection…");
    if (client.connect(mqtt_client, mqtt_username, mqtt_password)) {
      Serial.println("Connected");
      client.subscribe("@msg/LEDstatus");
    }
    else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println("Try again in 5 seconds...");
      delay(5000);
    }
  }
}

void onoff(int lighting) {
  if (lighting == 1){
    Serial.println("Turn on the light!");
    digitalWrite(relay,HIGH);
    client.publish("@shadow/data/update", "{\"data\" : {\"LEDstatus\" : \"on\"}}");
  } 
  else if (lighting == 0) {
    Serial.println("Turn off the light!");
    digitalWrite(relay,LOW);
    client.publish("@shadow/data/update", "{\"data\" : {\"LEDstatus\" : \"off\"}}");
  }  
}

void callback(char* topic,byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("]: ");
  String msg;
  for (int i = 0; i < length; i++) {
    msg = msg + (char)payload[i];
  }
   Serial.println(msg);
  if (String(topic) == "@msg/LEDstatus") {
    if (msg == "on"){
      lighting = 1;
      onoff(lighting); 
    } 
    else if (msg == "off") {
      lighting = 0;
      onoff(lighting);
    } 
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("HELLO :)");
  pinMode(relay,OUTPUT);
  pinMode(PIR,INPUT);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  if(millis() >= countTime + period) {
    PIRstate = digitalRead(PIR);
    countTime += period;
    if (PIRstate == LOW) {
      Serial.println("Motion absent");
    }
    else if (PIRstate == HIGH) {
      Serial.println("Motion detected :)");
      lighting = lighting + 1;
      if (lighting == 2) {
        lighting = 0;
      }
        onoff(lighting);  
      } 
    }
  client.loop();
}
