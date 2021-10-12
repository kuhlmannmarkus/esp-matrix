#include "matrix.h"

Matrix::Matrix() {
  this->m_wifi_client = NULL;
  this->m_http_client = NULL;
  this->m_accesstoken = "";
  setCallback(NULL);
  this->m_lastMessageToken = "";
}

Matrix::Matrix(WiFiClientSecure& _wificlient, HTTPClient& _httpclient) {
  this->m_http_client = &_httpclient;
  this->m_wifi_client = &_wificlient;
  this->m_accesstoken = "";
  setCallback(NULL);
  this->m_lastMessageToken = "";
}

Matrix::~Matrix() {
  
}

Matrix& Matrix::setCallback(MATRIX_CALLBACK_SIGNATURE) {
    this->callback = callback;
    return *this;
}

void Matrix::setDomain(const char* _domain) {
  this->m_domain = _domain;
  return;
}

void Matrix::setScheme(const char* _scheme){
  this->m_scheme = _scheme;
  return;
}

const char* Matrix::getDomain() {
  return this->m_domain;
}

const char* Matrix::getScheme() {
  return this->m_scheme;
}

bool Matrix::connect() {
  bool res = false;
  return res;
}

bool Matrix::login(const char* _username, const char* _password){
  bool res = false;
  String msg;
  DynamicJsonDocument root(256);
  root["type"] = "m.login.password";
  root["user"] = _username;
  root["password"] = _password;
  serializeJson(root, msg);
  String url = String(m_scheme) + String(SCHEME_SEPARATOR) + String(m_domain) + String(LOGIN_URL); //CAN WE USE SOMETHING ELSE THAN STRINGS
  m_http_client->begin(*m_wifi_client, url);
  m_http_client->addHeader("Content-Type", "application/json");
  int rc = m_http_client->POST(msg);
  if (rc > 0) {
    if (rc == HTTP_CODE_OK) {
      String body = m_http_client->getString();
      m_http_client->end();
      DynamicJsonDocument resp(256);
      deserializeJson(resp, body);
      m_accesstoken = String(resp["access_token"]);
      res = true;
    }
  }
  return res;
}

void Matrix::setAccessToken(String _accesstoken){
  this->m_accesstoken = _accesstoken;
}

String Matrix::getAccessToken(){
  return this->m_accesstoken;
}

bool Matrix::retrieve(const char* _roomid){
  bool res = true;
  String start = "something";
  String end = "somethingelse";
  while(start != end){
    String url = String(m_scheme) + String(SCHEME_SEPARATOR) + String(m_domain) + String(SEND_URL_P0) + String(_roomid) + "/messages?access_token=" + m_accesstoken + "&limit=1";
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
        res = false;
        m_http_client->end();
        return res;
      }
    }
    m_http_client->end();
  }
  return res;
}

bool Matrix::sendPlaintext(const char* _msg, const char* _roomid){
  String msgcontent;
  DynamicJsonDocument root(1024); //THIS IS DIRTY
  root["msgtype"] = "m.text";
  root["body"] = _msg;
  serializeJson(root, msgcontent);
  bool res = this->sendJSON(msgcontent.c_str(), _roomid);
  return res;
}

bool Matrix::sendMessage(Message _msg, const char* _roomid){
  bool res = false;
  return res;
}

bool Matrix::sendJSON(const char* _JSON, const char* _roomid){
  bool res = false;
  String url = String(m_scheme) + String(SCHEME_SEPARATOR) + String(m_domain) + String(SEND_URL_P0) + String(_roomid) + String(SEND_URL_P1) + String(millis()) + String("?access_token=") + m_accesstoken;
  m_http_client->begin(*m_wifi_client, url);
  m_http_client->addHeader("Content-Type", "application/json");
  int rc = m_http_client->PUT(_JSON);
  if (rc > 0) {
    if (rc == HTTP_CODE_OK) {
      res = true;
    }
  } 
  m_http_client->end();
  return res;
}
