#ifndef MATRIX_H
#define MATRIX_H

#define LOGIN_URL "/_matrix/client/r0/login"

#if defined(ESP8266) || defined(ESP32)
#include <functional>
#define MATRIX_CALLBACK_SIGNATURE std::function<void(char*)> callback
#else
#define MATRIX_CALLBACK_SIGNATURE void (*callback)(char*)
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
    void setDomain(const char*);
    void setScheme(const char*);
    const char* getDomain();
    const char* getScheme();
    bool login(const char*, const char*);
    void setAccessToken(const char*);
    const char* getAccessToken();
    bool retrieve();
    bool send(const char*);
    
  private:
    WiFiClientSecure* m_wifi_client;
    HTTPClient* m_http_client;
    const char* m_domain;
    const char* m_scheme;
    const char* m_accesstoken;
};

#endif // MATRIX_H
