//////////Includes/////////////

// WiFi connection

#include <ESP8266WiFi.h>

// MQTT CLient

#include <PubSubClient.h>

// CS811 Sensor

#include <Wire.h>    // I2C library
#include "ccs811.h"  // CCS811 library

// Wiring for ESP8266 NodeMCU boards: VDD to 3V3, GND to GND, SDA to D2, SCL to D1, nWAKE to D3 (or GND)
CCS811 ccs811(D3); // nWAKE on D3

// JSON to send data via MQTT

#include <ArduinoJson.h>

//////////Variables/////////////

// WiFi credentials
const char* ssid     = "*YourSSID*";
const char* password = "*YourWIFIPassword*";

// MQTT Client broker and credentials
const char* mqttBroker = "*YourMQTTBrokerIP*";
const int mqttPort = 1883;
const char* mqttUser = "*YourMQTTUser";
const char* mqttPassword = "*YourMQTTPassword*";
const char* sensor = "*YourSensorName";
const char* mqttPublishTopic = "*YourMQTTTopic*";

// Example MQTT Json message
const char* exampleMQTT = "{\"sensor\":\"esp32_01\",data\":[58.3,29.4,3.3],data\":[58.3,29.4,3.3]}";
// Calculate needed JSON document bytes with example message
const size_t CAPACITY = JSON_OBJECT_SIZE(sizeof(exampleMQTT) + 20); 

// DHT22 sensor type and which gpio pin is used for sensor data
// #define DHTPIN 27 
// #define DHTTYPE DHT22



// Creates WiFi instance and passes it on as a parameter for the MQTT client object
WiFiClient espClient;
PubSubClient client(espClient);


//////////Functions/////////////

// Function for setting up a WiFi connection
void setupWiFi()
{
    // Serial information
    Serial.println();
    Serial.println();
    Serial.print("STATUS: Connecting to ");
    Serial.println(ssid);

    // Initiates connection
    WiFi.begin(ssid, password);

    // As long as WiFi status is not connected a "loading bar" is displayed in serial terminal
    while (WiFi.status() != WL_CONNECTED) {
        delay(10000);
        Serial.print("#");
        WiFi.begin(ssid, password);
    }

    // WiFi status information
    Serial.println();
    Serial.println("STATUS: WiFi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    Serial.println();  
}

// Function for setting up MQTT client
void setupMqtt()
{
    // Defines MQTT broker and port
    client.setServer(mqttBroker, mqttPort);
    Serial.println("STATUS: Connecting to MQTT");
 
    // As long as MQTT client is not connected a "loading bar" is displayed in serial terminal
    while (!client.connected()) {
        delay(10000);
        Serial.print("#");
        client.connect("ESP32Client", mqttUser, mqttPassword);
    }

    // If client is connected some status information are displayed
    Serial.println();
    Serial.println("STATUS: Connected to MQTT");
    Serial.print("Server: ");
    Serial.println(mqttBroker);
    Serial.print("Port: ");
    Serial.println(mqttPort);
    Serial.println(); 
}

// Runs one time at startup
void setup()
{
    // Initializes serial terminal
    Serial.begin(115200);
    delay(10);

    // Calls function to set up WiFi connection
     setupWiFi();
    
    // Calls function to set up MQTT connection
     setupMqtt();  

    // Read the Values once and make sure the sensor works.

  // Enable I2C
  Wire.begin(); 
  
  // Enable CCS811
  ccs811.set_i2cdelay(50); // Needed for ESP8266 because it doesn't handle I2C clock stretch correctly
  bool ok= ccs811.begin();
  if( !ok ) Serial.println("setup: CCS811 begin FAILED");

  // Print CCS811 versions
  Serial.print("setup: hardware    version: "); Serial.println(ccs811.hardware_version(),HEX);
  Serial.print("setup: bootloader  version: "); Serial.println(ccs811.bootloader_version(),HEX);
  Serial.print("setup: application version: "); Serial.println(ccs811.application_version(),HEX);
  
  // Start measuring
  ok= ccs811.start(CCS811_MODE_1SEC);
  if( !ok ) Serial.println("setup: CCS811 start FAILED");

}

// After the intial Setup the loop delivers the measured values regular to the MQTT Server.

void loop()
    
{

  // Read values from the CS811 Sensor
  uint16_t eco2, etvoc, errstat, raw;
  ccs811.read(&eco2,&etvoc,&errstat,&raw); 


// Log the read values also to serial
    
    Serial.print("CCS811: ");
    Serial.print("eco2=");  Serial.print(eco2);     Serial.print(" ppm  ");
    Serial.print("etvoc="); Serial.print(etvoc);    Serial.print(" ppb  ");

    // Assign the read values to the variable.
    
    float veco2 = eco2;
    float vetvoc = etvoc;

    // Creates JSON document with capacity from example message. Contains sensor name and data. 
    
    StaticJsonDocument<CAPACITY> doc;
    doc["sensor"] = sensor;
    doc["eco2"] = veco2;
    doc["etvoc"] = vetvoc;
    
    // Serialize JSON doc to char buffer with variable capacity (MQTT client needs char / char*)
    
    char JSONmessageBuffer[CAPACITY];
    
    //serializeJson(doc, Serial);
    serializeJson(doc, JSONmessageBuffer);
    
    // Publishes JSON to defined MQTT topic
    while(!client.publish(mqttPublishTopic, JSONmessageBuffer)) {
        delay(10000);
        Serial.print("#MQTT#");
        if (!client.connected()){
          setupMqtt(); 
        }
    }
    Serial.println("STATUS: Sent data via MQTT");
    doc = NULL;


    Serial.flush();

  // Wait
  delay(20000); 

}
