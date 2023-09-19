


//These are the libraries we need. One makes the wifi connection process much easier. One connects to the MQTT server. The last one is just a manufacturer library
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

//This is the master delay that controls the cycle of the main loop
const int master_delay = 10000;
 
void setup() {
 
  Serial.begin(9600);
  dht.setup(14, DHTesp::DHT11); // GPIO14 setup for temperature sensor
  
  WiFi.begin(ssid, password); // connect to Wifi
 //Just waits for Wifi to be connected before continuing
  while (WiFi.status() != WL_CONNECTED) {
    delay(1100);
    Serial.println("Connecting to WiFi");
  }
  Serial.println("Connected to the WiFi");
 //connects to MQTT and sets the function that is called when a message is recieved.
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
 //Waits for MQTT to be connected before continuing
  while (!client.connected()) {
    Serial.println("Connecting to MQTT");
 
    if (client.connect("ESP8266Client" )) {
 
      Serial.println("connected");  
 
    } else {
      Serial.print(client.state());
      delay(2000);
 
    }
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      // Once connected, it publishes to a randomn topic and subscribes to a randomn topic to inialize the setup and makesure everything is working
      client.publish("randomn23/54", "hello world");
      client.subscribe("randomn/123/3245");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
//The callback function is the function that is called when a message is recieved. This isn't really necessary here since this will be used as a outgoing module only. 
//Nice to have if any future features is added. Such as response from the base or a display that shows the set temperature.
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
  //Gets temp and humidity values from the sensors
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
  //Converting from float to string. MQTT only accepts string values so a conversion must be done. 
  temp_str = String(temperature); 
  temp_str.toCharArray(temp, temp_str.length() + 1); 

  hum_str = String(humidity);
  hum_str.toCharArray(hum, hum_str.length() + 1);


  Serial.println(temp);
  Serial.println(hum);
  Serial.println(temp_str);
  Serial.println(hum_str);
  
  //Publish all of these values to the MQTT which will than be processed by the python script
  client.publish("esp/hum1",hum);
  Serial.println("Published Humidity");
  delay(2500);
  client.publish("esp/temp1",temp);
  Serial.println("Published Temperature");
}
