// Project: Smart_Home_IoT_System
// Author: Hemant Rathore
// Description: This code implements a smart home IoT system using ESP32, WiFi, MQTT, IR remote control, DHT sensor, motion sensor, and MQ2 gas sensor.
// Created Date: 2025-04-11
#include <WiFi.h>
#include <PubSubClient.h>
#include <IRremote.h>
#include <DHT.h>

// === WiFi & MQTT ===
const char* ssid = "Wokwi-GUEST";
const char* password = "";
const char* mqttServer = "demo.thingsboard.io";
const char* token = "gLpxYXede8M3Ps6cd2Qy";
const int mqttPort = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

// === Relay Configuration ===
const int relayPins[8] = {23, 22, 21, 4, 2, 15, 16, 17};
bool relayStates[8] = {true, true, true, true, true, true, true, true};

// === IR Receiver ===
#define IR_RECEIVE_PIN 35

// === DHT Sensor ===
#define DHT_PIN 25
#define DHT_TYPE DHT22
DHT dht(DHT_PIN, DHT_TYPE);
unsigned long lastDHTReadTime = 0;
const unsigned long dhtInterval = 1000;

// === Motion Sensor ===
#define MOTION_PIN 34
bool motionEnabled = true;
unsigned long lastMotionTime = 0;
const unsigned long motionDuration = 10 * 60 * 1000;
bool motionDetected = false;

// === MQ2 Sensor ===
#define MQ2_ANALOG_PIN 33
#define MQ2_DIGITAL_PIN 32
unsigned long lastGasReadTime = 0;
const unsigned long gasInterval = 1000;

void setup() {
  Serial.begin(115200);

  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");

  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);

  Serial.println("Starting IR Receiver...");
  IrReceiver.begin(IR_RECEIVE_PIN, DISABLE_LED_FEEDBACK);
  dht.begin();

  pinMode(MOTION_PIN, INPUT);
  pinMode(MQ2_DIGITAL_PIN, INPUT);

  for (int i = 0; i < 8; i++) {
    pinMode(relayPins[i], OUTPUT);
    digitalWrite(relayPins[i], relayStates[i] ? LOW : HIGH);
  }
}

void loop() {
  if (!client.connected()) reconnectMQTT();
  client.loop();

  // IR Remote Handling
  if (IrReceiver.decode()) {
    unsigned long command = IrReceiver.decodedIRData.command;
    Serial.print("IR Command: ");
    Serial.println(command, HEX);

    switch (command) {
      case 0x30: toggleRelay(0); break;
      case 0x18: toggleRelay(1); break;
      case 0x7A: toggleRelay(2); break;
      case 0x10: toggleRelay(3); break;
      case 0x38: toggleRelay(4); break;
      case 0x5A: toggleRelay(5); break;
      case 0x42: toggleRelay(6); break;
      case 0x4A: toggleRelay(7); break;
    }
    IrReceiver.resume();
  }

  // DHT22
  if (millis() - lastDHTReadTime >= dhtInterval) {
    lastDHTReadTime = millis();
    float temp = dht.readTemperature();
    float hum = dht.readHumidity();
    if (!isnan(temp) && !isnan(hum)) sendSensorData(temp, hum);
  }

  // Motion Sensor
  if (motionEnabled) {
    if (digitalRead(MOTION_PIN) == HIGH) {
      if (!motionDetected) {
        motionDetected = true;
        lastMotionTime = millis();
        setRelay(5, true);
      } else {
        lastMotionTime = millis();
      }
    } else if (motionDetected && millis() - lastMotionTime > motionDuration) {
      setRelay(5, false);
      motionDetected = false;
    }
  }

  // MQ2 Gas Sensor
  if (millis() - lastGasReadTime >= gasInterval) {
    lastGasReadTime = millis();
    int gasAnalog = analogRead(MQ2_ANALOG_PIN);
    bool gasDetected = digitalRead(MQ2_DIGITAL_PIN);

    String gasPayload = "{\"gasAnalog\":" + String(gasAnalog) + ",\"gasDetected\":" + (gasDetected ? "true" : "false") + "}";
    client.publish("v1/devices/me/telemetry", gasPayload.c_str());
  }
}

// === Relay Control ===
void toggleRelay(int index) {
  if (index < 0 || index >= 8) return;
  relayStates[index] = !relayStates[index];
  digitalWrite(relayPins[index], relayStates[index] ? HIGH : LOW);
  sendRelayStatus(index);
}

void setRelay(int index, bool state) {
  if (index < 0 || index >= 8) return;
  relayStates[index] = state;
  digitalWrite(relayPins[index], state ? HIGH : LOW);
  sendRelayStatus(index);
}

// === MQTT Functions ===
void reconnectMQTT() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32_Client", token, NULL)) {
      Serial.println("connected");
      client.subscribe("v1/devices/me/rpc/request/+");
    } else {
      Serial.print(" failed, rc=");
      Serial.print(client.state());
      Serial.println(" retrying in 2s...");
      delay(2000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  payload[length] = '\0';
  String data = String((char*)payload);
  Serial.print("MQTT message: ");
  Serial.println(data);

  int relayIndex = -1;
  for (int i = 0; i < 8; i++) {
    if (data.indexOf("\"method\":\"setRelay" + String(i + 1) + "\"") != -1) {
      relayIndex = i;
      break;
    }
  }

  if (relayIndex != -1) {
    bool state = data.indexOf("\"params\":true") != -1;
    setRelay(relayIndex, state);
  }
}

// === Telemetry ===
void sendSensorData(float temperature, float humidity) {
  String payload = "{\"temperature\":" + String(temperature) + ",\"humidity\":" + String(humidity) + "}";
  client.publish("v1/devices/me/telemetry", payload.c_str());
}

void sendRelayStatus(int index) {
  String key = "relay" + String(index + 1);
  String state = relayStates[index] ? "true" : "false";
  String attrPayload = "{\"" + key + "\":" + state + "}";
  client.publish("v1/devices/me/attributes", attrPayload.c_str());
  client.publish("v1/devices/me/telemetry", attrPayload.c_str());
}
