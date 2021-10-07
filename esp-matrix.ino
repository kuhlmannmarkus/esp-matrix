#define BAUDRATE 115200

#include "matrix.h"

Matrix *client;

void setup() {
  Serial.begin(BAUDRATE);
  Serial.println();
  client = new Matrix();
  client->setHost("Test");
  Serial.println(client->getHost());
  delete(client);
}

void loop() {

}
