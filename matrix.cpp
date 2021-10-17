#include "matrix.h"

Matrix::Matrix() {
  this->m_wifi_client = NULL;
  this->m_http_client = NULL;
  this->m_accesstoken = "";
  this->m_scheme = "";
  this->m_domain = "";
  setCallback(NULL);
  this->m_lastMessageToken = "";
}

Matrix::Matrix(WiFiClientSecure& _wificlient, HTTPClient& _httpclient) {
  this->m_http_client = &_httpclient;
  this->m_wifi_client = &_wificlient;
  this->m_accesstoken = "";
  this->m_scheme = "";
  this->m_domain = "";
  setCallback(NULL);
  this->m_lastMessageToken = "";
}

Matrix::~Matrix() {
  
}

Matrix& Matrix::setCallback(MATRIX_CALLBACK_SIGNATURE) {
    this->callback = callback;
    return *this;
}

void Matrix::setDomain(String _domain) {
  this->m_domain = _domain;
  return;
}

void Matrix::setScheme(String _scheme){
  this->m_scheme = _scheme;
  return;
}

bool Matrix::login(const char* _username, const char* _password){
  bool res = false;
  String msg;
  DynamicJsonDocument root(256);
  root["type"] = "m.login.password";
  root["user"] = _username;
  root["password"] = _password;
  serializeJson(root, msg);
  String url = m_scheme + String("://") + m_domain + String("/_matrix/client/r0/login");
  m_http_client->begin(*m_wifi_client, url);
  //IN MORE POMPLEX SCENARIOS (LDAP USER LOOKUP ETC.) ALLOW FOR LARGER TIMEOUT VALUES
  m_http_client->setTimeout(10000);
  m_http_client->addHeader("Content-Type", "application/json");
  int rc = m_http_client->POST(msg);
  if (rc > 0) {
    if (rc == HTTP_CODE_OK) {
      String body = m_http_client->getString();
      m_http_client->end();
      DynamicJsonDocument resp(256);
      deserializeJson(resp, body);
      this->m_accesstoken = String(resp["access_token"]);
      res = true;
    }
    else{
      Serial.print("ERROR: HTTP CODE: ");
      Serial.println(rc);
    }
  }
  else{
      Serial.print("ERROR: HTTP CLIENT RC: ");
      Serial.println(rc);
  }
  return res;
}

void Matrix::setAccessToken(String _accesstoken){
  this->m_accesstoken = _accesstoken;
}

bool Matrix::retrieve(const char* _roomid){
  bool res = true;
  String start = "something";
  String end = "somethingelse";
  while(start != end){
    String url = m_scheme + String("://") + m_domain + String("/_matrix/client/r0/rooms/") + String(_roomid) + "/messages?access_token=" + m_accesstoken + "&limit=1";
    if (m_lastMessageToken == "") {
      url += "&dir=b";
    }
    else {
      url += "&dir=f&from=" + String(m_lastMessageToken);
    }
    m_http_client->begin(*m_wifi_client, url);
    int rc = m_http_client->GET();
    if (rc > 0) {
      if (rc == HTTP_CODE_OK) {
        String cont = m_http_client->getString();
        DynamicJsonDocument doc(2048);
        deserializeJson(doc, cont);
        start = String(doc["start"]);
        end = String(doc["end"]);
        JsonObject chunk_0 = doc["chunk"][0];
        String ID = chunk_0["event_id"]; 
        String sender = chunk_0["user_id"];
        if(end != start && ID != m_latestMessageID){
          callback((char*)cont.c_str(), (char*)_roomid);
        }
        m_latestMessageID = ID;
        m_lastMessageToken = String(doc["end"]);
        m_lastMessageStartToken = String(doc["start"]);
      }
      else {
        Serial.print("ERROR: HTTP CODE: ");
        Serial.println(rc);
        res = false;
        m_http_client->end();
        return res;
      }
    }
    else{
      Serial.print("ERROR: HTTP CLIENT RC: ");
      Serial.println(rc);
    }
    m_http_client->end();
  }
  return res;
}

bool Matrix::sendPlaintext(const char* _msg, const char* _roomid){
  String msgcontent;
  DynamicJsonDocument root(1024); //THIS MIGHT BE DIRTY, DEPENDING ON THE LENGTH OF _msg
  root["msgtype"] = "m.text";
  root["body"] = _msg;
  serializeJson(root, msgcontent);
  bool res = this->sendJSON(msgcontent.c_str(), _roomid);
  return res;
}

bool Matrix::sendJSON(const char* _JSON, const char* _roomid){
  bool res = false;
  String url = m_scheme + String("://") + m_domain + String("/_matrix/client/r0/rooms/") + String(_roomid) + String("/send/m.room.message/") + String(millis()) + String("?access_token=") + m_accesstoken;
  m_http_client->begin(*m_wifi_client, url);
  m_http_client->addHeader("Content-Type", "application/json");
  int rc = m_http_client->PUT(_JSON);
  if (rc > 0) {
    if (rc == HTTP_CODE_OK) {
      res = true;
    }
    else{
      Serial.print("ERROR: HTTP CODE: ");
      Serial.println(rc);
    }
  }
  else{
      Serial.print("ERROR: HTTP CLIENT RC: ");
      Serial.println(rc);
  }
  m_http_client->end();
  return res;
}
