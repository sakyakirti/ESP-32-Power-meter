#include <EmonLib.h>
#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#define vCalibration 83.3
#define currCalibration 0.5
#define MAX 2600
//Deklarasi WIFI
const char* wifiSSID = "TP-Link_83B6";
const char* wifiPassword = "67273225";
//Deklarasi MQTT
const char* mqtt_server =  "192.168.0.100";//IPv4 Static
const char* mqttUsername = "root";
const char* mqttPassword = "";
//Deklarasi Data
String Vrms();
String Irms();
String apparentPower();
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
  Serial.print(emon.Irms, 2);
  Serial.print("A");
  Serial.print("\tPower: ");
  Serial.print(emon.apparentPower, 4);
  Serial.print("W");
  Serial.print("\tkWh: ");
  kWh = kWh + emon.apparentPower * (millis() - lastmillis) / 3600000000.0;
  Serial.print(kWh, 6);
  Serial.println("kWh");
  lastmillis = millis();
  if (!client.connected())
  {
    connect_mqtt();
    Serial.println("MQTT Connected");
    client.publish("esp32/energy_meter", "ESP 32 Online!");
  }
   client.loop();
    client.publish("volt",    Vrms().c_str());
    client.publish("ampere",  Irms().c_str());
    client.publish("daya",   apparentPower().c_str());
    client.publish("kwh",     String(kWh).c_str());
    delay(1000);
}

String Vrms()
{
  int Vrms = emon.Vrms;
  Serial.print(emon.Vrms, 2);
  Serial.println("V");
  return String(Vrms);
}
String Irms()
{
  int Irms = emon.Irms*1000;
  Serial.print(emon.Irms*1000, 2);
  Serial.println("mA");
  return String(Irms);
}
String apparentPower()
{
  int apparentPower = emon.apparentPower;
  Serial.print(emon.apparentPower, 2);
  Serial.println("W");
  return String(apparentPower);
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
