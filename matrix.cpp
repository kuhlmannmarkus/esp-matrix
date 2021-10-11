#include "matrix.h"

Matrix::Matrix() {
  this->m_wifi_client = NULL;
  this->m_http_client = NULL;
  this->m_accesstoken = NULL;
  setCallback(NULL);
}

Matrix::Matrix(WiFiClientSecure& _wificlient, HTTPClient& _httpclient) {
  this->m_http_client = &_httpclient;
  this->m_wifi_client = &_wificlient;
  this->m_accesstoken = NULL;
  setCallback(NULL);
}

Matrix::~Matrix() {
  
}

Matrix& Matrix::setCallback(MATRIX_CALLBACK_SIGNATURE) {
    this->callback = callback;
    return *this;
}

void Matrix::test() {
  callback("HUHU", "LALA");
  return;
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

bool Matrix::login(const char* _username, const char* _password){
  bool res = false;
  String msg;
  DynamicJsonDocument root(1024); //THIS IS DIRTY
  root["type"] = "m.login.password";
  root["user"] = _username;
  root["password"] = _password;
  serializeJson(root, msg);
  String url = String(m_scheme) + String("://") + String(m_domain) + String(LOGIN_URL); //CAN WE USE SOMETHING ELSE THAN STRINGS
  m_http_client->begin(*m_wifi_client, url);
  m_http_client->addHeader("Content-Type", "application/json");
  int rc = m_http_client->POST(msg);
  if (rc > 0) {
    if (rc == HTTP_CODE_OK) {
      String body = m_http_client->getString();
      m_http_client->end();
      DynamicJsonDocument resp(1024); //THIS IS DIRTY
      deserializeJson(resp, body);
      m_accesstoken = resp["access_token"];
      res = true;
    }
  }
  return res;
}

void Matrix::setAccessToken(const char* _accesstoken){
  this->m_accesstoken = _accesstoken;
}

const char* Matrix::getAccessToken(){
  return this->m_accesstoken;
}

bool Matrix::retrieve(){
  bool res = false;
  return res;
}

bool Matrix::sendPlaintext(const char* _msg, const char* _roomid){
  String msgcontent;
  DynamicJsonDocument root(1024);
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
  String url = String(m_scheme) + String("://") + String(m_domain) + String(SEND_URL0) + String(_roomid) + String(SEND_URL1) + String(millis()) + String("?access_token=") + String(m_accesstoken);
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
