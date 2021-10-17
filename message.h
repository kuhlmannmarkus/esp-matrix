#ifndef MESSAGE_H
#define MESSAGE_H

#include <ArduinoJson.h>

class Message {
  public:
    Message();
    Message(const char*);
    ~Message();
    String getBody();
    String getSender();
    String getRoom();
    String getFormattedBody();
    bool hasIDBeenMentioned(const char*);
    bool isFromSender(const char*);
    String getCommand();
    String getJSONForSending();
    
  private:
    const char* m_type;
    const char* m_room_id;
    const char* m_sender;
    const char* m_msgtype;
    const char* m_body;
    const char* m_format;
    const char* m_formatted_body;
    long m_origin_server_ts;
    const char* m_event_id;
    const char* m_user_id;
    unsigned int m_age;
    unsigned int m_unsigned_age;
    const char* m_start;
    const char* m_end;
};

#endif // MESSAGE_H
