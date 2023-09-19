//This is the code for the temperature module.

//These are the libraries we need. One makes the wifi connection process much easier. One connects to the MQTT server. The last one is just a manufacturer library for the temperature sensor
//for temperature sensor.
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHTesp.h>
DHTesp dht;
WiFiClient espClient;
PubSubClient client(espClient);

//These are the details for my Wifi and MQTT so it can connect.
const char* ssid = "Maheshwari1";
const char* password =  "Hy3erabad";
const char* mqttServer = "broker.emqx.io";
const int mqttPort = 1883;
const char* mqttUser = "admin";
const char* mqttPassword = "hivemq";

// variables for float to string conversions later
String temp_str;
String hum_str;
char temp[50];
char hum[50];

//These are the pins for RGB LED

int RED = 5;
int GREEN = 4;
int BLUE = 0;

//This is the master delay that controls the cycle of the main loop. Currently it is set for 10 seconds.
const int master_delay = 10000;
 
void setup() {
 
  Serial.begin(9600);
  dht.setup(14, DHTesp::DHT11); //Setup for temperature sensor

  //Pin setup for RGB LED
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);

  WiFi.begin(ssid, password); // connect to Wifi
 //Just waits for Wifi to be connected before continuing
  while (WiFi.status() != WL_CONNECTED) {
    delay(1100);
    Serial.println("Connecting to WiFi");
    digitalWrite(RED,LOW);
    digitalWrite(GREEN,LOW);
    digitalWrite(BLUE,BLUE);
  }
  Serial.println("Connected to the WiFi");
  digitalWrite(RED,HIGH);
  digitalWrite(GREEN,HIGH);
  digitalWrite(BLUE,LOW);
 //connects to MQTT and sets the function that is called when a message is recieved.
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
 //Waits for MQTT to be connected before continuing. This is basic code that the documentation of the ESP wifi provides, 
 //however I added the waiting aspect to ensure that it has connected.
  while (!client.connected()) {
    Serial.println("Connecting to MQTT");
 
    if (client.connect("ESP8266Client" )) {
 
      Serial.println("connected");  
      digitalWrite(RED,LOW);
      digitalWrite(GREEN,HIGH);
      digitalWrite(BLUE,LOW);
    } else {
      Serial.print(client.state());
      delay(2000);
 
    }
  }
}
//I got the idea for the recconect function from the example code however it didn't work so I sued a couple of lines from there but I remade the rest of the function
void reconnect() {
  // Loop until we're reconnected
  digitalWrite(RED,HIGH);
  digitalWrite(GREEN,HIGH);
  digitalWrite(BLUE,LOW);
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection");
    // Attempt to connect
    if (client.connect("ESP8266Client2")) {
      Serial.println("connected");
      // Once connected, it publishes to a randomn topic and subscribes to a randomn topic to inialize the setup and makesure everything is working
      client.publish("randomn23/54", "hello world");
      client.subscribe("randomn/123/3245");
      digitalWrite(RED,LOW);
      digitalWrite(GREEN,HIGH);
      digitalWrite(BLUE,LOW);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
//The callback function is the function that is called when a message is recieved. This isn't really necessary here since this will be used as a outgoing module only. 
//Nice to have if any future features is added. Such as response from the base or a display that shows the set temperature. This function is a modified
//version from the example. 
void callback(char* topic, byte* payload, unsigned int length) {
 
  Serial.print("Topic: ");
  Serial.println(topic);
 
  Serial.print("Message:");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
 
  Serial.println();
  Serial.println("______________________");
 
}
 
void loop() {
  //checks if mqqt is connected and if not, it uses the reconnect function.
  if (!client.connected()) {
    reconnect();
  }
  //This begins the listening process after the recconect if there has been one. Not necessary right now but could be needed.
  client.loop();
  //This is the master delay and it controls the 
  delay(master_delay);
  //Gets temp and humidity values from the sensors. This is just the modified version of the example code from the library. 
  float humidity = dht.getHumidity();
  float temperature = dht.getTemperature();
  Serial.print("Status: ");
  Serial.print(dht.getStatusString());
  Serial.print("\t");
  Serial.print("Humidity (%): ");
  Serial.print(humidity, 1);
  Serial.print("\t");
  Serial.print("Temperature (C): ");
  Serial.print(temperature, 1);
  Serial.print("\t");
  Serial.println();
  
  //Converting from float to string. MQTT only accepts string values so a conversion must be done. This is a modified version of a conversion formula I found on the Arduino blog. 
  temp_str = String(temperature); 
  temp_str.toCharArray(temp, temp_str.length() + 1); 

  hum_str = String(humidity);
  hum_str.toCharArray(hum, hum_str.length() + 1);

  Serial.println(temp);
  Serial.println(hum);
  Serial.println(temp_str);
  Serial.println(hum_str);
  
  //Publish all of these values to the MQTT which will than be processed by the python script
  client.publish("esp/hum2",hum);
  Serial.println("Published Humidity");
  delay(2500);
  client.publish("esp/temp2",temp);
  Serial.println("Published Temperature");
}
