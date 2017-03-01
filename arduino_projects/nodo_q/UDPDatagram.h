#ifndef __UDPDATAGRAM__
#define __UDPDATAGRAM__

#include <Arduino.h>
#include "QCA7000MacLayer.h"
#include "IPv6_packet.h"


class UDPDatagram {
  private:
    uint16_t srcPort;
    uint16_t destPort;
    uint16_t checksum;
    uint8_t *buffer;
    uint8_t *payload;
    uint16_t payloadLength;
    IPv6Packet& ipPacketAssociated;

  public:

    static const uint8_t UDP_HEADER_LENGTH = 8;
    static const uint8_t UDP_NEXT_HEADER   = 17;


    UDPDatagram(IPv6Packet& myIPPacket) : ipPacketAssociated(myIPPacket) {
      //ipPacketAssociated = myIPPacket;
      setBuffer( myIPPacket.getBuffer() + IPv6Packet::IPV6_HEADER_LENGTH );
    }

    UDPDatagram(IPv6Packet& myIPPacket, const uint16_t src_port, const uint16_t dest_port) : ipPacketAssociated(myIPPacket) {
      //      ipPacketAssociated = myIPPacket;

      
      setBuffer( myIPPacket.getBuffer() + IPv6Packet::IPV6_HEADER_LENGTH );
      setSourcePort( src_port );
      setDestinationPort( dest_port );

    }



    bool send(uint8_t *data, const uint16_t dataLength) {
      ipPacketAssociated.setNextHeader( UDP_NEXT_HEADER );
      setPayload( data, dataLength );
      prepareDatagram();
      //Serial.println("Prepare bien el datagrama");
      //delay(100);
      return ipPacketAssociated.send(buffer - UDP_HEADER_LENGTH, dataLength + UDP_HEADER_LENGTH );
    }

    void setPayload( uint8_t *data, const uint16_t dataLength) {
      payload = data;
      payloadLength = dataLength;
    }

    void setBuffer(uint8_t *myBuffer) {
      buffer = myBuffer;
    }

    void getPayload( uint8_t *data, uint16_t *dataLength) {
      data = payload;
      *dataLength = payloadLength;
    }

    void setSourcePort( uint16_t src_port ) {
      srcPort = src_port;
    }

    void setDestinationPort( uint16_t dest_port ) {
      destPort = dest_port;
    }

    void prepareDatagram() {

      

      buffer[0] = (uint8_t) (srcPort >> 8);
      buffer[1] = (uint8_t) (srcPort & (0x00FF));

      buffer[2] = (uint8_t) (destPort >> 8);
      buffer[3] = (uint8_t) (destPort & (0x00FF));

      const uint16_t UDPlength = payloadLength + UDP_HEADER_LENGTH;
      buffer[4] = (uint8_t) (UDPlength >> 8);
      buffer[5] = (uint8_t) (UDPlength & (0x00FF));

      ipPacketAssociated.setNextHeader( UDP_NEXT_HEADER );
      checksum = calculateUDPChecksum();
      buffer[6] = (uint8_t) (checksum >> 8);
      buffer[7] = (uint8_t) (checksum & (0x00FF));

      memcpy(buffer + UDP_HEADER_LENGTH, payload, payloadLength);

    }



    uint16_t calculateUDPChecksum() {
      uint32_t retorno32 = 0;
      uint16_t retorno = 0;
      uint8_t *auxIP;
      uint16_t i;

      auxIP = ipPacketAssociated.getSourceIP();
      for ( i = 0; (i < IPv6Packet::IP_LEN); i += 2 ) {
        retorno32 += word(auxIP[i], auxIP[i + 1]);
      }

      auxIP = ipPacketAssociated.getDestinationIP();
      for ( i = 0; (i < IPv6Packet::IP_LEN); i += 2 ) {
        retorno32 += word(auxIP[i], auxIP[i + 1]);
      }

      retorno32 += ipPacketAssociated.getNextHeader();

      uint32_t lengthUDP = UDP_HEADER_LENGTH + payloadLength;
      retorno32 += lengthUDP & 0x0000FFFF;
      retorno32 += lengthUDP >> 16;
      retorno32 += lengthUDP & 0x0000FFFF;
      retorno32 += lengthUDP >> 16;

      retorno32 += srcPort;
      retorno32 += destPort;

      for ( i = 0; i < (payloadLength - 2); i += 2 ) {
        retorno32 += word(payload[i], payload[i + 1]);
      }
      //Si es par falta la suma final;
      if ( payloadLength % 2 == 0 ) {
        retorno32 += word( payload[ payloadLength - 2 ], payload[ payloadLength - 1 ] );
      } else {
        retorno32 += word( payload[ payloadLength - 1 ], 0 );
      }

      retorno = (uint16_t) ( ( retorno32 & 0xFFFF0000 ) >> 16 );
      retorno += (uint16_t) ( retorno32 & 0x0000FFFF );
      retorno ^= 0xFFFF;

      return retorno;

    }
    
};

#endif

