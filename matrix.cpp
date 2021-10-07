#include "matrix.h"

Matrix::Matrix() {
  this->m_client = NULL;
}

Matrix::Matrix(HTTPClient* _client) {
  this->m_client = _client;
}

Matrix::~Matrix() {
  
}

void Matrix::setPollingInterval(unsigned int _ms) {
  this->m_pollinginterval = _ms;
}

void Matrix::setHost(const char* _host) {
  this->m_host = _host;
}

const char* Matrix::getHost() {
  return this->m_host;
}

bool Matrix::login(const char* _username, const char* _password){
  bool res = false;
  return res;
}
