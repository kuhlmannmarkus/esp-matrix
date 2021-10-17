#ifndef MATRIX_H
#define MATRIX_H

#if defined(ESP8266) || defined(ESP32)
#include <functional>
#define MATRIX_CALLBACK_SIGNATURE std::function<void(char*, char*)> callback
#else
#define MATRIX_CALLBACK_SIGNATURE void (*callback)(char*, char*)
#endif

#include "Arduino.h"
#include <ArduinoJson.h>
#include <WiFiClientSecure.h>
#include <ESP8266HTTPClient.h>

class Matrix {
  public:
    Matrix();
    Matrix(WiFiClientSecure&, HTTPClient&);
    ~Matrix();
    void setDomain(String);
    void setScheme(String);
    bool login(const char*, const char*);
    void setAccessToken(String);
    bool retrieve(const char*);
    bool sendPlaintext(const char*, const char*);
    Matrix& setCallback(MATRIX_CALLBACK_SIGNATURE);
    
  private:
    bool sendJSON(const char*, const char*);
    WiFiClientSecure* m_wifi_client;
    HTTPClient* m_http_client;
    String m_domain;
    String m_scheme;
    String m_accesstoken;
    String m_lastMessageToken;
    String m_lastMessageStartToken;
    String m_latestMessageID;
    bool m_ready;
    MATRIX_CALLBACK_SIGNATURE;
};

#endif // MATRIX_H
