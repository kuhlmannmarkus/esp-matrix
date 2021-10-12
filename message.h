#ifndef MESSAGE_H
#define MESSAGE_H

class Message {
  public:
    Message();
    Message(const char*);
    ~Message();
    void setPlainMessage(const char*);
    const char* getPlainMessage();
  private:
    const char* m_plainMessage;
    const char* m_sender;
};

#endif // MESSAGE_H
