#include "QCA7000MACLayer.h"
#include "IPv6_packet.h"
#include "UDPDatagram.h"
#include "ICMP_RADV.h"
#include <ArduinoJson.h>

typedef uint8_t buffer_512[512];

buffer_512 sendBuffer;
buffer_512 receiveBuffer;

const byte self_mac_address[6] = {0x00, 0x01, 0x87, 0x09, 0x02, 0xA3};

QCA7000MACLayer macLayer(self_mac_address, sendBuffer);

IPv6Packet recibir(macLayer);
IPv6Packet enviar(macLayer);

bool configured = false;
uint8_t routerAddress[IPv6Packet::IP_LEN] = {0};

#define DEBUG 1

#define LOCAL_AMNT  2;
const int local_ids[LOCAL_AMNT] = {0, 1};
const int local_types[LOCAL_AMNT] = {'c', 'r'};

void setup() {
  for ( char i = 0; i < LOCAL_AMNT; i++ ) {
    if ( local_types(i) == 'c' ) {
      pinMode(local_ids[i], INPUT);
    } else {
      pinMode(local_ids[i], OUTPUT);
    }
  }
  analogReference(EXTERNAL);
  analogRead(A0);
  Serial.begin(115200);

  if ( !macLayer.init() ) {
#if DEBUG
    Serial.println("Fallo el connect al spi");
#endif
    while (true);
    //In future implementations, should reset all hardware.
  }
#if DEBUG
  Serial.println("Connect OK");
#endif

}

unsigned long prevM = millis();

void loop() {
  uint8_t nextHeader = recibir.receive();
  if ( nextHeader != IPV6_HEADER_TYPE_UNKOWN ) {

    if ( configured ) {
      if ( nextHeader == IPV6_HEADER_TYPE_UDP ) {
        json_parse_packet_send_action(recibir);
      }
    } else {

      ICMP_RADV myRadv(recibir);
      if ( myRadv.isRouterAdvertisement() ) {
        memcpy( routerAddress, recibir.getSourceIP(), IPv6Packet::IP_LEN);
        configured = true;
#if DEBUG
        Serial.println("CONFIGURADO, IP DEL ROUTER: ");
        for ( char i = 0; i < IPv6Packet::IP_LEN; i++ ) {
          Serial.print("| ");
          Serial.print(recibir.getSourceIP()[i], HEX);
        }
        Serial.println("");
#endif
      }

    }

  }

  if ( (millis() - prevM) >= 4000 ) {
    for ( char i = 0; i < LOCAL_AMNT; i++) {
      if ( local_types[i] == 'c' ) {
        int measurement = take_measurement(local_ids[i]);
        json_send_measurement(recibir, local_ids[i], measurement);
      }
    }
    prevM = millis();
  }
}


void qca_interrupt_handler() {
  macLayer.interruptHandler();
}

void testSendIP() {

  IPv6Packet paquete(macLayer);

  const uint8_t ip_dest[16] = {0xFE, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x01, 0x87, 0xFF, 0xFE, 0x09, 0x02, 0x65};

  uint8_t ip_src[16];
  IPv6Packet::getIPv6LinkLocalAddressFromMacAddress(macLayer.getMacAddress(), ip_src);

  uint8_t data[50];
  for ( char i = 0; i < 50; i++ ) {
    data[ i ] = 0x11;
  }

  IPv6Packet miPaquete(macLayer, ip_dest, ip_src);
  Serial.println( miPaquete.send(data, 50) == true ? "Mando OK" : "Error al enviar");

}


void testSendUDP() {

  const uint8_t ip_dest[16] = {0xFE, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x01, 0x87, 0xFF, 0xFE, 0x09, 0x02, 0x65};

  uint8_t ip_src[16];
  IPv6Packet::getIPv6LinkLocalAddressFromMacAddress(macLayer.getMacAddress(), ip_src);

  uint8_t data[51];
  for ( char i = 0; i < 51; i++ ) {
    data[ i ] = 0x30;
  }

  Serial.println("Creando paquete ");
  delay(100);
  IPv6Packet miPaquete(macLayer, ip_dest, ip_src );
  Serial.println("Paquete creado ");
  delay(100);

  UDPDatagram miDatagrama(miPaquete, 65000, 65000);

  Serial.println("Enviando datagrama");
  delay(100);
  miDatagrama.send(data, 50);


}

int take_measurement(int local_id) {
  return analogRead(local_ids[local_id]);
}

void turn_off(int local_id) {

}

void turn_on(int local_id) {

}

