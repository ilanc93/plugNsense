#ifndef __QCA7000_COMMONS__
#define __QCA7000_COMMONS__

#include <Arduino.h>

#define CHECK_BIT(var,pos) (((var)>>(pos)) & 1)

const uint16_t QCA_SPI_REG_BFR_SIZE        = 0x0100;
const uint16_t QCA_SPI_REG_WRBUF_SPC_AVA   = 0x0200;
const uint16_t QCA_SPI_REG_RDBUF_BYTE_AVA  = 0x0300;
const uint16_t QCA_SPI_REG_SPI_CONFIG      = 0x0400;
const uint16_t QCA_SPI_REG_INTR_CAUSE      = 0x0C00;
const uint16_t QCA_SPI_REG_INTR_ENABLE     = 0x0D00;
const uint16_t QCA_SPI_REG_SIGNATURE       = 0x1A00;

const uint8_t  QCA_SPI_SLAVE_RESET_BIT_POS = 6;

const int8_t QCA_SPI_SOF_LEN  = 4;
const int8_t QCA_SPI_FL_LEN   = 2;
const int8_t QCA_SPI_RSVD_LEN = 2;
const int8_t QCA_SPI_EOF_LEN  = 2;
const int8_t QCA_SPI_HW_LEN   = 4;

const uint16_t QCA_SPI_CONF_REG_EXTERNAL_WRITE  = 0;
const uint16_t QCA_SPI_CONF_REG_EXTERNAL_READ   = 1 << 15;

const uint16_t QCA_SPI_REG_SIGNATURE_VALUE      = 0xAA55;


const int8_t ETHERNET_MIN_FRAME_SIZE = 60;
const uint16_t ETHERNET_MAX_FRAME_SIZE = 1522;

const uint8_t ETHERNET_ETHERTYPE_HIGH = 0x86;
const uint8_t ETHERNET_ETHERTYPE_LOW  = 0xDD;
const uint8_t ETHERNET_ETHERTYPE_LENGTH  = 2;
const uint8_t ETHERNET_HEADER_LENGTH  = 14;

//Este buffer es lo que deberia ser el start of frame.
const uint8_t QCA_SPI_READ_CHECK_SOF[QCA_SPI_SOF_LEN] = {0xAA, 0xAA, 0xAA, 0xAA};
const uint8_t QCA_SPI_READ_CHECK_RSVD[QCA_SPI_RSVD_LEN] = {0x00, 0x00};
const uint8_t QCA_SPI_READ_CHECK_EOF[QCA_SPI_EOF_LEN] = {0x55, 0x55};

typedef union {
  uint16_t value;
  uint8_t valueArray[2];
} myUInt16_t;

typedef union {
  uint32_t value;
  uint8_t valueArray[4];
} myUInt32_t;


typedef struct {
  const uint8_t startOfFrame[QCA_SPI_SOF_LEN] = {0xAA, 0xAA, 0xAA, 0xAA};
  myUInt16_t frameLength;
  const uint8_t rsvd[QCA_SPI_RSVD_LEN] = {0x00, 0x00};
  uint8_t frameBody[ETHERNET_MAX_FRAME_SIZE] = { 0x00, 0x01, 0x87, 0x09, 0x02, 0x6F, 0x00, 0x01, 0x87, 0x09, 0x02, 0x65, 0x08, 0x00, 0x45, 0x00, 0x00, 0x54, 0xE6, 0x08, 0x00, 0x00, 0x40, 0x01, 0xD3, 0x3E, 0xC0, 0xA8, 0x01, 0x6B, 0x40, 0xE9, 0xBE, 0x65, 0x08, 0x00, 0x6C, 0xA5, 0xC2, 0x1F, 0x00, 0x00, 0x58, 0x02, 0xB4, 0xCA, 0x00, 0x0A, 0xD1, 0x60, 0x08, 0x09, 0x0A, 0x0B, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37};
  const uint8_t endOfFrame[QCA_SPI_EOF_LEN] = {0x55, 0x55};
} qca_transmit_frame;


typedef struct {
  myUInt32_t packetLength;
  uint8_t startOfFrame[QCA_SPI_SOF_LEN];
  myUInt16_t frameLength;
  uint8_t rsvd[QCA_SPI_RSVD_LEN];
  uint8_t frameBody[ETHERNET_MAX_FRAME_SIZE];
  uint8_t endOfFrame[QCA_SPI_EOF_LEN];
} qca_receive_frame;

/*
typedef union {
  qca_transmit_frame tx;
  qca_receive_frame rx;
} qca_spi_frame;
*/


/*
    REGISTROS SPI QCA7000
*/


#endif
