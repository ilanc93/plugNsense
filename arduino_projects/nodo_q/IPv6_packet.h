#ifndef __IPV6_PACKET__
#define __IPV6_PACKET__

#include <Arduino.h>
#include "QCA7000MacLayer.h"


#define IPV6_HEADER_TYPE_UDP     17
#define IPV6_HEADER_TYPE_ICMPV6  58
#define IPV6_HEADER_TYPE_UNKOWN  255

class IPv6Packet {

  public:

    static const uint8_t IP_LEN                   = 16;
    static const uint8_t IPV6_HEADER_LENGTH       = 40;
    static const uint8_t IPV6_MAX_PAYLOAD_LENGTH  = 458;

  private:

    const uint8_t version_trafficClass_flowLabel[4] = {0x60, 0x00, 0x00, 0x00};
    int16_t payloadLength;
    uint8_t nextHeader;
    uint8_t hopLimit = 64;
    uint8_t sourceAddress[IP_LEN];
    uint8_t destinationAddress[IP_LEN];
    uint8_t *payload;
    uint8_t *buffer;
    QCA7000MACLayer& macLayerAssociated;

    void preparePacketHeader() {
      memcpy(buffer, version_trafficClass_flowLabel, 4);
      buffer[4] = (uint8_t) (payloadLength >> 8);
      buffer[5] = (uint8_t) (payloadLength & (0x00FF));
      buffer[6] = nextHeader;
      buffer[7] = hopLimit;
      memcpy(buffer + 8, sourceAddress, IP_LEN);
      memcpy(buffer + 24, destinationAddress, IP_LEN);
    }

  public:
    IPv6Packet(QCA7000MACLayer& myMacLayer) : macLayerAssociated(myMacLayer) {
      macLayerAssociated = myMacLayer;
      setBuffer( myMacLayer.getBuffer() );
      getIPv6LinkLocalAddressFromMacAddress( macLayerAssociated.getMacAddress(), sourceAddress );

    }

    IPv6Packet(QCA7000MACLayer& myMacLayer, const uint8_t *ip_dest, const uint8_t *ip_src) : macLayerAssociated(myMacLayer) {
      macLayerAssociated = myMacLayer;
      setBuffer( myMacLayer.getBuffer()  );
      setDestinationIP( ip_dest );
      setSourceIP( ip_src );

    }

    bool send(uint8_t *data, const uint16_t dataLength) {
      Serial.println("Preparando el paquete");
      delay(100);
      payloadLength = dataLength;
      preparePacketHeader();
      uint8_t mac_dest[UIP_LLADDR_LEN];
      getMacAddressFromIPv6LinkLocalAddress( mac_dest, destinationAddress);
      Serial.println("Prepare bien el paquete");
      delay(100);
      return macLayerAssociated.send(mac_dest, buffer, dataLength + IPV6_HEADER_LENGTH) == MAC_TX_STATUS_OK ? true : false;
    }

    void setDestinationIP( const uint8_t *ip_dest) {
      memcpy(destinationAddress, ip_dest, IP_LEN );
    }

    void setSourceIP( const uint8_t *ip_src) {
      memcpy(sourceAddress, ip_src, IP_LEN );
    }

    void setPayload( uint8_t *data, const uint16_t dataLength) {
      payload = data;
      payloadLength = dataLength;
    }

    void setNextHeader( const uint8_t myNextHeader ) {
      nextHeader =  myNextHeader;
    }

    void setBuffer(uint8_t *myBuffer) {
      buffer = myBuffer;
    }

    uint8_t* getDestinationIP() {
      return destinationAddress;
    }

    uint8_t* getSourceIP() {
      return sourceAddress;
    }

    void getPayload( uint8_t *data, uint16_t *dataLength) {
      //data = payload;
      *dataLength = payloadLength;
    }

    uint8_t* getBuffer() {
      return buffer;
    }

    uint8_t* getPayload() {
      return buffer + IPV6_HEADER_LENGTH;
    }


    uint8_t getNextHeader() {
      return nextHeader;
    }


    /*
      Mac address:  XX:XX:XX:XX:XX:XX
      IPv6 Link local address: fe80::xyxx:xxff:fexx:xxxx
      Where y = X XOR 2.
    */
    static void getIPv6LinkLocalAddressFromMacAddress(const uint8_t *mac_address, uint8_t* ip) {
      ip[0] = 0xFE;
      ip[1] = 0x80;
      ip[2] = 0x00;
      ip[3] = 0x00;
      ip[4] = 0x00;
      ip[5] = 0x00;
      ip[6] = 0x00;
      ip[7] = 0x00;
      ip[8] = (mac_address[0]) ^ 0x02;
      ip[9] = mac_address[1];
      ip[10] = mac_address[2];
      ip[11] = 0xFF;
      ip[12] = 0xFE;
      ip[13] = mac_address[3];
      ip[14] = mac_address[4];
      ip[15] = mac_address[5];
    }

    /*
        Mac address:  XX:XX:XX:XX:XX:XX
        IPv6 Link local address: fe80::xyxx:xxff:fexx:xxxx
        Where y = X XOR 2.
    */
    static void getMacAddressFromIPv6LinkLocalAddress(uint8_t* mac_address, const uint8_t* ip) {
      mac_address[0] = ip[8] ^ 0x02;
      mac_address[1] = ip[9];
      mac_address[2] = ip[10];
      mac_address[3] = ip[13];
      mac_address[4] = ip[14];
      mac_address[5] = ip[15];
    }

    //Devuelve el next header y -1 si es no reconocido
    uint8_t receive() {
      uint8_t mac_src[6];
      uint8_t mac_dest[6];
      uint16_t buffLen;
      if ( macLayerAssociated.receive( mac_src, mac_dest, buffer, &buffLen) ) {
        if ( parsePacket( mac_src, mac_dest, buffLen ) ) {
          Serial.println("6");
          return nextHeader;
        } else {
          setNextHeader( IPV6_HEADER_TYPE_UNKOWN );
          return nextHeader;
        }
      } else {
        setNextHeader( IPV6_HEADER_TYPE_UNKOWN );
        return nextHeader;
      }
    }

    bool parsePacket( uint8_t *mac_src, uint8_t *mac_dest, uint16_t buffLen ) {
      int16_t i; //Variable auxiliar para los for
      Serial.println("4");
      for ( i = 0; i < 4; i++ ) {
        if ( version_trafficClass_flowLabel[i] != buffer[i] ) {
          setNextHeader( IPV6_HEADER_TYPE_UNKOWN );
          /*
          Serial.println("Me fui por version traffic class flow label");
          for ( i = 0; i < 4; i++ ) {
            Serial.println(buffer[i], HEX);
          }
          */
          return false;
        }
      }


      if ( buffLen < IPV6_HEADER_LENGTH ) {
        setNextHeader( IPV6_HEADER_TYPE_UNKOWN );
        Serial.println("Me fui por header length");
        return false;
      }

      getIPv6LinkLocalAddressFromMacAddress( mac_src, sourceAddress);
      //Chequea que la IP sea la de link local. Sino devuelve false.
      for ( i = 0; i < IP_LEN; i++ ) {
        if ( buffer[ 8 + i ] != sourceAddress[ i ] ) {
          setNextHeader( IPV6_HEADER_TYPE_UNKOWN );
          Serial.println("Me fui por IP de origen");
          return false;
        }
      }

      for ( i = 0; i < IP_LEN; i++ ) {
        destinationAddress[ i ] = buffer[ 24 + i ];
      }

      payloadLength = (buffer[4] << 8) + buffer[5];
      if ( payloadLength > IPV6_MAX_PAYLOAD_LENGTH ) {
        setNextHeader( IPV6_HEADER_TYPE_UNKOWN );
        Serial.println("Me fui por payload length");
        return false;
      }

      setNextHeader( buffer[6] );
      //Aca se tienen que meter todos los headers conocidos;
      switch ( buffer[6] ) {
        case IPV6_HEADER_TYPE_UDP:
        case IPV6_HEADER_TYPE_ICMPV6:
          break;
        default:
          Serial.println("Me fui por next header"); 
          setNextHeader( IPV6_HEADER_TYPE_UNKOWN );
          return false;
      }

      return true;
    }
};


#endif
