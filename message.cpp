#include "message.h"

Message::Message() {
  
}

Message::Message(const char* _body) {
  DynamicJsonDocument doc(1024); //THIS MIGHT BE DIRTY, DEPENDING ON THE LENGTH OF _body
  deserializeJson(doc, String(_body));
  
  JsonObject chunk_0 = doc["chunk"][0];
  m_type = chunk_0["type"];
  m_room_id = chunk_0["room_id"];
  m_sender = chunk_0["sender"];

  JsonObject chunk_0_content = chunk_0["content"];
  m_msgtype = chunk_0_content["msgtype"];
  m_body = chunk_0_content["body"];
  if (chunk_0_content.containsKey("format")){
    m_format = chunk_0_content["format"]; 
  }
  else{
   m_format = NULL; 
  }
  
  if (chunk_0_content.containsKey("formatted_body")){
    m_formatted_body = chunk_0_content["formatted_body"];
  }
  else{
   m_formatted_body = NULL; 
  }
  

  m_origin_server_ts = chunk_0["origin_server_ts"];

  m_unsigned_age = chunk_0["unsigned"]["age"];

  m_event_id = chunk_0["event_id"];
  m_user_id = chunk_0["user_id"];
  m_age = chunk_0["age"]; // 169

  m_start = doc["start"];
  m_end = doc["end"];
  
}

Message::~Message() {
  
}

String Message::getBody(){
  return String(m_body);
}

String Message::getSender(){
  return String(m_sender);
}

String Message::getFormattedBody(){
  return String(m_formatted_body);
}

bool Message::hasIDBeenMentioned(const char* _id){
  bool res = false;
  String id = "<a href=\"https://matrix.to/#/" + String(_id);
  int selfidpos = String(m_formatted_body).indexOf(id);
  if(selfidpos != -1){
    res = true;
  }
  return res;
}

bool Message::isFromSender(const char* _id){
  bool res = false;
  if(strcmp(m_sender, _id) == 0){
    res = true;
  }
  return res;
}

//I DONT LIKE THIS, BUT IT WORKS FOR MESSAGES SENT BY ELEMENT
String Message::getCommand(){
  String html = String(m_formatted_body);
  while(html.indexOf("<a") != -1){
    int pos1 = html.indexOf("<a");
    int pos2 = html.indexOf("</a>");
    String repl = html.substring(pos1, pos2+4);
    html.replace(repl, "");
  }
  if(html.charAt(0) == ':'){
    html = html.substring(1, html.length());
  }
  
  html.trim();
  return html;
}

String Message::getRoom(){
  return String(m_room_id);
}

String Message::getJSONForSending(){
  String res;
  return res;
}
