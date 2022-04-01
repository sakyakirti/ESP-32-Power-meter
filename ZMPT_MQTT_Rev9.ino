#include <EmonLib.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <PubSubClient.h>
#define vCalibration 83.3
#define currCalibration 0.5
#define MAX 2600
//Deklarasi WIFI
const char* wifiSSID = "TP-Link_83B6";//TP-Link_83B6
const char* wifiPassword = "";//67273225
//Deklarasi MQTT
const char* mqtt_server =  "192.168.0.100";//IPv4 Static
const char* mqttUsername = "root";
const char* mqttPassword = "";
//Deklarasi Data
float kWh = 0;
String valuekWh = String(kWh);
unsigned long lastmillis = millis();
WiFiClient espClient;
PubSubClient client(espClient);
EnergyMonitor emon;
//Deklarasi Fungsi
void connectWifi();
void connect_mqtt();
void setup() {
  Serial.begin(115200);
  emon.voltage(34, vCalibration, 1.7); // Input dari ZMPT101B
  emon.current(35, currCalibration); // Input dari SCT013 30A
  connectWifi();
  client.setServer(mqtt_server, 3307);
}
void loop() {
  emon.calcVI(20, 2000);
  Serial.print("Vrms: ");
  Serial.print(emon.Vrms, 2);
  Serial.print("V");
  Serial.print("\tIrms: ");
  Serial.print(emon.Irms*1000, 2);
  Serial.print("A");
  Serial.print("\tPower: ");
  Serial.print(emon.apparentPower, 4);
  Serial.print("W");
  Serial.print("\tkWh: ");
  kWh = kWh + emon.apparentPower * (millis() - lastmillis) / 3600000000.0;
  Serial.print(kWh, 6);
  Serial.println("kWh");
  delay(3000);
  lastmillis = millis();

DynamicJsonDocument doc(200);
  char json[] =
  "{\"emon.Vrms\":\"V\",\"emon.Irms\":\"I\",\"emon.apparentPower\":\"P\",\"kWh\":\"W\"}";
DeserializationError error = deserializeJson(doc, json);
   if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  const char* Vrms = doc["Vrms"];
  const char* Irms = doc["Irms"];
  const char* apparentPower = doc["apparentPower"];
  String(kWh) = doc["kWh"];
  long time = doc["time"];
  Serial.print("Parsing emon.Vrms : ");
  Serial.println(Vrms);
  Serial.print(emon.Vrms, 2);
  Serial.println("V");
  Serial.print("Parsing emon.Irms: ");
  Serial.println(Irms);
  Serial.print(emon.Irms*1000, 2);
  Serial.println("mA");
  Serial.print("Parsing emon.apparentPower: ");
  Serial.println(apparentPower);
  Serial.print(emon.apparentPower, 2);
  Serial.println("P");
  Serial.print("Parsing kWh: ");
  Serial.println(kWh);
  }
  if (!client.connected())
  {
    connect_mqtt();
    Serial.println("MQTT Connected");
    client.publish("esp32/energy_meter", "ESP 32 Online!");
}
   client.loop();
    client.publish("volt",  doc["Vrms"]);
    client.publish("amp",   doc["Irms"]);
    client.publish("daya",  doc["apparentPower"]);
    client.publish("kwh",   doc["kWh"]);
    delay(5000);
}
void connect_mqtt() {
  while (!client.connected())
  {
    Serial.println("Connecting MQTT...");
    if (client.connect("esp32"))
    {
    client.subscribe("esp32/energy meter");
    }
  }
}
void connectWifi() {
  Serial.println("Connecting To Wifi");
  WiFi.begin(wifiSSID, wifiPassword);
  while (WiFi.status() != WL_CONNECTED)
  {
  Serial.print(".");
  delay(500);
  }
  Serial.println("Wifi Connected");
  Serial.println(WiFi.SSID());
  Serial.println(WiFi.RSSI());
  Serial.println(WiFi.macAddress());
  Serial.println(WiFi.localIP());
  Serial.println(WiFi.gatewayIP());
  Serial.println(WiFi.dnsIP());
}
