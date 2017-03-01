#ifndef __ICMP_RADV__
#define __ICMP_RADV__

#include <Arduino.h>
#include "QCA7000MacLayer.h"
#include "IPv6_packet.h"


class ICMP_RADV {
  private:
    uint8_t type;
    IPv6Packet& ipPacketAssociated;
    const uint8_t ICMPv6_TYPE_RADV = 134;
  public:

    ICMP_RADV(IPv6Packet& myIPPacket) : ipPacketAssociated(myIPPacket) {
      //ipPacketAssociated = myIPPacket;      
    }

    bool isRouterAdvertisement(){
      if ( ipPacketAssociated.getNextHeader() != IPV6_HEADER_TYPE_ICMPV6 ){
        return false;
      }
      type = ipPacketAssociated.getPayload()[0];
      if ( type != ICMPv6_TYPE_RADV ){
        return false;
      }
      return true;
    }
    

    
};

#endif

