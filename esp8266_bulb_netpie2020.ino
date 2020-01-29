#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define PIR 15
#define relay 4
#define LED 13

const char* ssid = "TU608083";
const char* password = "29396128";
const char* mqtt_server = "broker.netpie.io";
const int mqtt_port = 1883;
const char* mqtt_Client = "88f6ad3d-4511-4da1-9a34-fde4f0d0ecf6";
const char* mqtt_username = "hGoE7u7zNpbG6Jmyb4JqsPXeAKq5iNYs";
const char* mqtt_password = "-+v@XV9wRewI&#.2Su&6uk9mf!N$&&Xd";
int lighting = 0;
int statePIR = 0;
int stateLED = 0;

WiFiClient espClient;
PubSubClient client(espClient);

char msg[50];

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection…");
    if (client.connect(mqtt_Client, mqtt_username, mqtt_password)) {
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
      convertState(msg); 
  }
}

void convertState(String msg) {
  if (msg == "on"){
    lighting = 1;
  } 
  else if (msg == "off") {
    lighting = 0;
  }
  onoff(lighting); 
}

void onoff(int lighting) {
  if (lighting == 1){
    Serial.println("Turn on the light!");
    digitalWrite(LED,HIGH);
    digitalWrite(relay,HIGH);
    client.publish("@shadow/data/update", "{\"data\" : {\"LEDstatus\" : \"on\"}}");
  } 
  else if (lighting == 0) {
    Serial.println("Turn off the light!");
    digitalWrite(LED,LOW);
    digitalWrite(relay,LOW);
    client.publish("@shadow/data/update", "{\"data\" : {\"LEDstatus\" : \"off\"}}");
  }  
}

void setup() {
  Serial.begin(115200);
  Serial.println("HELLO :)");
  pinMode(relay,OUTPUT);
  pinMode(LED,OUTPUT);
  pinMode(PIR,INPUT);
  
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
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
    statePIR = digitalRead(PIR);
    //int stateLED = digitalRead(LED);

    if (statePIR == LOW) {
      Serial.println("Motion absent");
      delay(1200);
    }
    else if (statePIR == HIGH) {
      Serial.println("Motion detected :)");
      lighting = lighting + 1;
      if (lighting == 2) {
        lighting = 0;
      }  
      onoff(lighting);
      delay(6000);
    }
    client.loop();
}

//D8 D2 D7
