# Process CS811 sensor data via ESP32 microcontroller and publish data to a MQTT Server.

ESP32 nodeMCU connects to WiFi and MQTT broker, sends sensor data in JSON format via MQTT.

Forked from https://github.com/JonasMock/ESP32_MQTT_DHT22_Sensor .
To connect the sensor the follwoing library is used for the NodeMCU https://github.com/maarten-pennings/CCS811 .

Make sure the Sensor is wired in the follwing way: 

Sensor --- ESP32 (NodeMCU)
GND --- GND, VCC --- 3V, SDA --- D2, SCL --- D1, WAKE --- GND

Important Information about the CS811 Sensor:

Although the readings tend to be good enough in the long run, introducing non-respiration sources of VOC or CO2, such as peeling fruits or using perfume, hand disinfactant, will undermine their reliability. (https://en.wikipedia.org/wiki/Carbon_dioxide_sensor)


