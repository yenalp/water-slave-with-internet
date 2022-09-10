#include <SPI.h>
#include <WiFiNINA.h>
#include <ArduinoHttpClient.h>
#include <ArduinoJson.h>

#define comSerial Serial1
String percentageString = "";
char ssid[] = ""; 
char pass[] = "";
int status = WL_IDLE_STATUS;

char serverAddress[] = "";
int port = 443;

WiFiSSLClient wifi;
HttpClient client = HttpClient(wifi, serverAddress, port);
String response;
int statusCode = 0;

void setup() {
  Serial.begin(9600);
  Serial.println();
  comSerial.begin(4800);

  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

  // Attempt to connect to WiFi network:
  connectToWifi();

  // you're connected now, so print out the data:
  Serial.print("You're connected to the network");
  printCurrentNet();
  printWifiData();

}

void loop() {

  // attempt to connect to WiFi network:
  connectToWifi();

  // Read incoming serial
  while (comSerial.available()) {
    if (comSerial.available() > 0) {
      percentageString = comSerial.read();
    }
  }

  // Only send when we have a value
  if (percentageString.length() > 0 ) {

    // If we get a reading greater than 100
    int percentInt = percentageString.toInt();
    if (percentInt > 100) {
      percentageString = 100;
    }

    // Print to serial port
    Serial.print("percentage: ");
    Serial.println(percentageString);
    Serial.println("===================");

    StaticJsonDocument<200> json;
    json["zone"] = "outside";
    json["water"] = percentageString;

    // Send the data 
    String postData;
    serializeJson(json, postData);

    client.beginRequest();
    client.post("/v1/outside");
    client.sendHeader("Content-Type", "application/json");
    client.sendHeader("Content-Length", postData.length());
    client.sendHeader("x-api-key", "dWGajIpe8Lafbg2c3qm14acNbX1ZwKCvaLgc62H1");
    client.beginBody();
    client.print(postData);
    client.endRequest();

    statusCode = client.responseStatusCode();
    response = client.responseBody();
    percentageString = "";
  }
}

void connectToWifi() {
  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }
}

void printCurrentNet() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print the MAC address of the router you're attached to:
  byte bssid[6];
  WiFi.BSSID(bssid);
  Serial.print("BSSID: ");
  printMacAddress(bssid);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.println(rssi);

  // print the encryption type:
  byte encryption = WiFi.encryptionType();
  Serial.print("Encryption Type:");
  Serial.println(encryption, HEX);
  Serial.println();
}

void printWifiData() {
  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  Serial.println(ip);

  // print your MAC address:
  byte mac[6];
  WiFi.macAddress(mac);
  Serial.print("MAC address: ");
  printMacAddress(mac);
}
void printMacAddress(byte mac[]) {
  for (int i = 5; i >= 0; i--) {
    if (mac[i] < 16) {
      Serial.print("0");
    }
    Serial.print(mac[i], HEX);
    if (i > 0) {
      Serial.print(":");
    }
  }
  Serial.println();
}
