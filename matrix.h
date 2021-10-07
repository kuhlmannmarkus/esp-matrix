#ifndef MATRIX_H
#define MATRIX_H

#if defined(ESP8266) || defined(ESP32)
#include <functional>
#define MATRIX_CALLBACK_SIGNATURE std::function<void(char*)> callback
#else
#define MATRIX_CALLBACK_SIGNATURE void (*callback)(char*)
#endif

#include "Arduino.h"
#include <ESP8266HTTPClient.h>

class Matrix {
  public:
    Matrix();
    Matrix(HTTPClient*);
    ~Matrix();
    void setPollingInterval(unsigned int);
    void setHost(const char*);
    const char* getHost();
    bool login(const char*, const char*);
    
  private:
    HTTPClient* m_client;
    const char* m_host;
    int m_pollinginterval;
    const char* m_accesstoken;
};

#endif // MATRIX_H
