#ifndef __QCA7000MACLAYER__
#define __QCA7000MACLAYER__

#include "MACLayer.h"
#include "QCA7000_commons.h"
#include "QCA7000_conf.h"
#include <Arduino.h>
#include <SPI.h>

/*
 Needs to be declared outside the class and, assuming a QCA7000MACLayer object named myMacLayer, should be
  void qca_interrupt_handler(){
    myMacLayer.interruptHandler();
  }
 */
extern void qca_interrupt_handler();

class QCA7000MACLayer: public MACLayer{
    private:
      uint8_t my_mac[UIP_LLADDR_LEN];
      bool getResponseMAC();
      uint8_t *buffer;
      uint8_t src_mac_buffer[UIP_LLADDR_LEN];
      uint8_t dest_mac_buffer[UIP_LLADDR_LEN];
      uint16_t bufferLen;
  
      uint16_t qca_spi_readRegister(const uint16_t spi_register);
      uint16_t qca_spi_writeRegister(const uint16_t spi_register, const uint16_t register_value);
  
      inline void qca_spi_startExternalWrite();
      inline void qca_spi_startExternalRead();
      bool packetAvailable();
      inline void attach_Interrupt();
      bool frameToRead = false;
      bool readParseFrame();
  
  public:
      QCA7000MACLayer(const uint8_t mac[UIP_LLADDR_LEN], uint8_t *myBuffer);
  
      bool init();
      MACTransmissionStatus send(const uint8_t* lladdr_dest, uint8_t* data, uint16_t length);
      bool receive(uint8_t* lladdr_src, uint8_t* lladdr_dest, uint8_t* data, uint16_t* length);
      const uint8_t* getMacAddress();
  
  
    void interruptHandler();
  
    uint8_t* getBuffer();
  
};

#endif

