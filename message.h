//
// Copyright (c) 2021 Markus Kuhlmann - kuhlmann.markus@gmail.com
//
// Refer to the LICENSE file for licensing information
//

#ifndef MESSAGE_H
#define MESSAGE_H

class Message {
  public:
    Message();
    ~Message();
    void setPlainMessage(const char*);
    const char* getPlainMessage();
  private:
    const char* m_plainMessage;
};

#endif // MESSAGE_H
