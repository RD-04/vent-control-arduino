//This is the code for the servo module. 

//Libraries for Wifi, MQTT, and servo
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Servo.h>

Servo servo;
WiFiClient espClient;
PubSubClient client(espClient);

//These are the details for my Wifi and MQTT so it can connect.
const char* ssid = "Maheshwari1";
const char* password =  "Hy3erabad";
const char* mqttServer = "broker.emqx.io";
const int mqttPort = 1883;

//variables for servo position
String servo_angle = "";
int servo_pos = 0;
int old_servo=0;
int servo_open = 180;
int servo_close = 0;
int servo_half = 45;
int servo_extra = 145;

 
void setup() {
  //All basic setup for serial monitor, and servo. The servo is set to it's default state at startup. 
  //
  Serial.begin(9600);

  servo.attach(2); //D4
  
  servo.write(old_servo);
  //connect to Wifi. This part is based on the basic WiFi library for ESP but I added the waiting aspect to increase reliability.
  WiFi.begin(ssid, password);
  //Wait for Wifi connection before continuing 
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi");
  }
  Serial.println("Connected to the WiFi");
  
  //connects to MQTT and sets the function that is called when a message is recieved.
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
  //Waits for MQTT to be connected before continuing. Modififed version of the connection process form the example 
  while (!client.connected()) {
    Serial.println("Connecting to MQTT");
 
    if (client.connect("ESP8266Client")) {
 
      Serial.println("connected");  
 
    } else {
      Serial.print(client.state());
      delay(2000);
 
    }
  }
  //subscribes to the topic that the servo position will be uploaded to. When using a public MQTT network, it is important for the 
  //topic to be specific so no one else can publish to it by accident
  
  client.subscribe("esp/servo1");
 
}
//This is the function called when message is recieved. It also checks what the message is and sets the servo position. 
//Modified from mqtt library example
void callback(char* topic, byte* payload, unsigned int length) {
 
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
 
  Serial.print("Message:");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  
  Serial.println();
  Serial.println("-----------------------");
  //The python scripts sends the position. We check the  first letter and  determine the angle that the servo should be in.
  if ((char)payload[0] == 'o') {
    servo_pos = servo_open;
    servo.write(servo_pos);
    Serial.println(servo_pos);
  }
  if ((char)payload[0] == 'h') {
    servo_pos = servo_half;
//    servo.write(servo_pos);
//    Serial.println(servo_pos);
    if (servo_pos == servo_close) {
      servo.write(160);
      Serial.println("halfway from closed");
    }
    if (servo_pos == servo_open) {
      servo.write(15);
      Serial.println("halfway from open");
    }
  }
  if ((char)payload[0] == 'c') {
    servo_pos = servo_close;
    servo.write(servo_pos);
    Serial.println(servo_pos);
  }
  if ((char)payload[0] == '3') {
    servo_pos = servo_extra;
    servo.write(servo_pos);
    Serial.println(servo_pos);
  }
  
}
//I got the idea for the recconect function from the example code however it didn't work so I used a couple of lines from there but I remade the rest of the function 
void reconnect() {
  // If there is a reconnect than this attempts a reconnect
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID and retries which can help reset the server and bypass some issues.
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publishes an randomn announcemnt and connects to our wanted topic once again
      client.publish("random123/23231", "hello world");
      client.subscribe("esp/servo1");
    } else {
      Serial.print(client.state());
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void loop() {
  //If it disconnected than reconnect. Else just listen for messages
  if (!client.connected()) {
    reconnect();
  }
  
  client.loop();
  
 
}
