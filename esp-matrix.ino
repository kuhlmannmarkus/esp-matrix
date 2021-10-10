#define BAUDRATE 115200
#define MAXWIFICONNECTIONWAIT 10000 //MILLISECONDS
#define WIFICONNECTIONWAITPERIOD 1000 //MILLISECONDS

#include <ESP8266WiFi.h>

#include "secrets.h"
#include "matrix.h"
#include "Arduino.h"
#include <ArduinoJson.h>
#include <WiFiClientSecure.h>
#include <ESP8266HTTPClient.h>

Matrix *client;
WiFiClientSecure wifiClient;
HTTPClient httpClient;

time_t now;

const char PMEMCACERT[] PROGMEM = CACERT;
X509List cacert(PMEMCACERT);

void setup() {
  //SERIAL LINK SETUP
  Serial.begin(BAUDRATE);
  Serial.println();
  
  //WIFI SETUP
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, WIFIPASSWORD);
  Serial.print("INFO: Local MAC address: ");
  Serial.println(WiFi.macAddress());
  Serial.print("INFO: Connecting to WiFi ");
  int connection_wait_periods = 0;
  while (WiFi.status() != WL_CONNECTED) {
    if(connection_wait_periods * WIFICONNECTIONWAITPERIOD > MAXWIFICONNECTIONWAIT){
      Serial.println("ERROR: WiFi connection failed");
      return;
    }
    Serial.print(".");
    connection_wait_periods++;
    delay(WIFICONNECTIONWAITPERIOD);
  }
  Serial.println();
  Serial.print("INFO: Connected to WiFi SSID: ");
  Serial.println(SSID);
  Serial.print("INFO: Local IP address: ");
  Serial.println(WiFi.localIP());

  //OPEN CONNECTION TO HOMESERVER
  //TODO CAN BE MOVED TO MATRIX OBJECT
  if(strcmp(HOMESERVER_URL_SCHEME, "https") == 0){
    configTime(3 * 3600, 0, "pool.ntp.org", "time.nist.gov");
    now = time(nullptr);
    while (now < 8 * 3600 * 2) {
      delay(500);
      now = time(nullptr);
    }
    struct tm timeinfo;
    gmtime_r(&now, &timeinfo);
    wifiClient.setTrustAnchors(&cacert);
  }
  if (!wifiClient.connect(HOMESERVER_DOMAIN, HOMESERVER_PORT)) {
    Serial.println("ERROR: Homeserver connection failed.");
    return;
  }
  else{
    Serial.println("INFO: Successfully connected to homeserver.");
  }
  //MATRIX CLIENT SETUP
  client = new Matrix(wifiClient, httpClient);
  client->setDomain(HOMESERVER_DOMAIN);
  client->setScheme(HOMESERVER_URL_SCHEME);
  //LOGIN METHOD 1: RETRIEVE ACCESS TOKEN
  bool loginsuccess = client->login(USERNAME, PASSWORD);
  if(!loginsuccess){
    Serial.println("ERROR: Homeserver login failed");
    return;
  }
  else{
    Serial.println("INFO: Successfully logged in.");
  }
  //LOGIN METHOD 2: DIRECTLY SET ACCESS TOKEN
  //client->setAccessToken(SOME_ACCESS_TOKEN);
}

void loop() {

}
