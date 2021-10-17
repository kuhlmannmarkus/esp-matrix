#define BAUDRATE 115200
#define MAXWIFICONNECTIONWAIT 10000 //MILLISECONDS
#define WIFICONNECTIONWAITPERIOD 1000 //MILLISECONDS

#include <ESP8266WiFi.h>

#include "secrets.h"
#include "matrix.h"
#include "message.h"
#include <ArduinoJson.h>
#include <WiFiClientSecure.h>
#include <ESP8266HTTPClient.h>
#include "DHT.h"

//DHT settings
#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

//Client objects
WiFiClientSecure wifiClient;
HTTPClient httpClient;
Matrix client(wifiClient, httpClient);

//TLS settings
String scheme = "http";
time_t now;
const char PMEMCACERT[] PROGMEM = CACERT;
X509List cacert(CACERT);
const char PMEMCLIENTCERT[] PROGMEM = CLIENTCERT;
X509List clientcert(PMEMCLIENTCERT);
const char PMEMCLIENTKEY[] PROGMEM = CLIENTKEY;
PrivateKey clientkey(PMEMCLIENTKEY);

void setup() {
  //LED setup
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  
  //DHT SETUP
  dht.begin();
  
  //SERIAL LINK SETUP
  Serial.begin(BAUDRATE);
  Serial.println();
  
  //WIFI SETUP
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, WIFIPASSWORD);
  Serial.print("INFO: Local MAC address: ");
  Serial.println(WiFi.macAddress());
  Serial.print("INFO: Connecting to WiFi ");
  unsigned int connection_wait_periods = 0;
  while (WiFi.status() != WL_CONNECTED) {
    if(connection_wait_periods * WIFICONNECTIONWAITPERIOD > MAXWIFICONNECTIONWAIT){
      Serial.println("ERROR: WiFi connection failed.");
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
  if(strcmp(CACERT, "") != 0){
    scheme = "https";
    configTime(3 * 3600, 0, "pool.ntp.org", "time.nist.gov");
    now = time(nullptr);
    while (now < 8 * 3600 * 2) {
      delay(500);
      now = time(nullptr);
    }
    struct tm timeinfo;
    gmtime_r(&now, &timeinfo);
    wifiClient.setTrustAnchors(&cacert);
    if(strcmp(CLIENTCERT, "") != 0 && strcmp(CLIENTKEY, "") != 0){
      wifiClient.setClientRSACert(&clientcert, &clientkey);
    }
    
  }
  if (!wifiClient.connect(HOMESERVER_DOMAIN, HOMESERVER_PORT)) {
    Serial.println("ERROR: Homeserver connection failed.");
    return;
  }
  else{
    Serial.println("INFO: Successfully connected to homeserver.");
  }
  
  //MATRIX CLIENT SETUP
  client.setDomain(HOMESERVER_DOMAIN);
  client.setScheme(scheme);
  //LOGIN METHOD 1: RETRIEVE ACCESS TOKEN
  bool loginsuccess = client.login(USERNAME, PASSWORD);
  if(!loginsuccess){
    Serial.println("ERROR: Homeserver login failed.");
    return;
  }
  else{
    Serial.println("INFO: Successfully logged in.");
  }
  //LOGIN METHOD 2: DIRECTLY SET ACCESS TOKEN
  //client.setAccessToken(ACCESS_TOKEN);
  client.setCallback(callback);
  delay(500);
}

//The callback that is triggered by the Matrix Client. Here you can apply filters.
void callback(char* _body, char* _roomid){
  Message msg(_body);
  String selfid = String("@") + String(USERNAME) + String(":") + String(HOMESERVER_DOMAIN);
  if(!msg.isFromSender(selfid.c_str()) && msg.hasIDBeenMentioned(selfid.c_str())){
    processMessage(msg);
  }
  return;
}

//Process a message and react to commands
void processMessage(Message _msg){
  String command = _msg.getCommand();
  if(strcmp(command.c_str(),"TEMP") == 0){
    String temp = getTemp();
    client.sendPlaintext(temp.c_str(), ROOMID);
  }
  else if(strcmp(command.c_str(),"HUM") == 0){
    String hum = getHum();
    client.sendPlaintext(hum.c_str(), ROOMID);
  }
  else if(strcmp(command.c_str(),"ON") == 0){
    digitalWrite(LED_BUILTIN, LOW);
    client.sendPlaintext("LED ON", ROOMID);
  }
  else if(strcmp(command.c_str(),"OFF") == 0){
    digitalWrite(LED_BUILTIN, HIGH);
    client.sendPlaintext("LED OFF", ROOMID);
  }
  else if(strcmp(command.c_str(),"STATE") == 0){
    bool state = digitalRead(LED_BUILTIN);
    String res = "LED IS ";
    if(state){
      res += "OFF";
    }
    else{
      res += "ON";
    }
    client.sendPlaintext(res.c_str(), ROOMID);
  }
  else{
    client.sendPlaintext("UNKNOWN COMMAND", ROOMID);
  }
  Serial.print("INFO: Processed command: \"");
  Serial.print(command);
  Serial.println("\"");
}

String getTemp(){
  float temp = dht.readTemperature();
  String res = "Temperature: " + String(temp) + "°C";
  return res;
}

String getHum(){
  float hum = dht.readHumidity();
  String res = "Relative humidity: " + String(hum) + "%";
  return res;
}

void loop() {
  bool retsuccess = client.retrieve(ROOMID);
  if(!retsuccess){
    Serial.println("ERROR: Retrieving messages failed.");
  }
  else{
    Serial.print("INFO: Successfully retrieved messages from room ");
    Serial.print(ROOMID);
    Serial.println(".");
  }
  delay(1000);
}
