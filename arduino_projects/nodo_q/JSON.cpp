#include "QCA7000MACLayer.h"
#include "IPv6_packet.h"
#include "UDPDatagram.h"
#include "ICMP_RADV.h"
#include <ArduinoJson.h>

extern void turn_off(int);
extern void turn_on(int);

extern const int local_ids[LOCAL_AMNT];
extern const int local_types[LOCAL_AMNT];


void json_parse_packet_send_action(IPv6Packet miPaquete){
  UDPDatagram miDatagrama(miPaquete);

  uint8_t* payload = miDatagrama.getPayload();
  String input = String((char *) payload);
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(input);
  if ( root.success() && root["type"] == "directive" ){
    int local_id = root["local_id"];
    if ( root["directive"] == String(1) ){
      turn_off(local_id);
    }else{
      turn_on(local_id);
    }
  }
}


void json_send_measurement(IPv6Packet miPaquete, int local_id, int measurement){
  
  UDPDatagram miDatagrama(miPaquete);
  uint8_t* payload = miDatagrama.getPayload();
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root = jsonBuffer.createObject();
  root["type"] = "measurements";
  root["local_id"] = local_id;
  JsonArray& data = root.createNestedArray("data");
  data.add(measurement);
  String json_string;
  root.printTo(json_string);
  json_string.getBytes(payload, json_string.length());
  miDatagrama.setPayload(payload, json_string.length());
  miDatagrama.send(miDatagrama.getPayLoad(), miDatagrama.getPayloadLength());
}

void json_send_configuration(IPv6Packet miPaquete){
  
  UDPDatagram miDatagrama(miPaquete);
  uint8_t* payload = miDatagrama.getPayload();
  StaticJsonBuffer<500> jsonBuffer;
  
  JsonObject& root = jsonBuffer.createObject();
  root["type"] = "configuration";
  JsonArray& data = root.createNestedArray("data");
  for ( char i = 0; i < LOCAL_AMNT; i++){
    JsonObject& current_io = jsonBuffer.createObject();
    current_io["local_id"] = local_ids[i];
    current_io["io_type"] = local_types[i] == 'c' ? 1 : 0;  
  }
  
  data.add(measurement);
  String json_string;
  root.printTo(json_string);
  json_string.getBytes(payload, json_string.length());
  miDatagrama.setPayload(payload, json_string.length());
  miDatagrama.send(miDatagrama.getPayLoad(), miDatagrama.getPayloadLength());
}


