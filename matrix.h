#ifndef MATRIX_H
#define MATRIX_H

#define LOGIN_URL "/_matrix/client/r0/login"
#define SCHEME_SEPARATOR "://"
#define SEND_URL_P0 "/_matrix/client/r0/rooms/"
#define SEND_URL_P1 "/send/m.room.message/"

#define STATUSCODE_HTTP_OK 0 //Im planning on returning ints instead of bools
#define STATUSCODE_HTTP_FAILED 1

#define STATUSCODE_CLIENT_OK 0
#define STATUSCODE_CLIENT_CONNECTION_FAILED 1

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
#include "message.h"

class Matrix {
  public:
    Matrix();
    Matrix(WiFiClientSecure&, HTTPClient&);
    ~Matrix();
    void setDomain(const char*);
    void setScheme(const char*);
    const char* getDomain();
    const char* getScheme();
    bool connect();
    bool login(const char*, const char*);
    void setAccessToken(String);
    String getAccessToken();
    bool retrieve(const char*);
    bool sendPlaintext(const char*, const char*);
    bool sendMessage(Message, const char*);
    Matrix& setCallback(MATRIX_CALLBACK_SIGNATURE);
    
  private:
    bool sendJSON(const char*, const char*);
    WiFiClientSecure* m_wifi_client;
    HTTPClient* m_http_client;
    const char* m_domain;
    const char* m_scheme;
    String m_accesstoken;
    String m_lastMessageToken;
    String m_lastMessageStartToken;
    String m_latestMessageID;
    bool m_ready;
    MATRIX_CALLBACK_SIGNATURE;
};

#endif // MATRIX_H
