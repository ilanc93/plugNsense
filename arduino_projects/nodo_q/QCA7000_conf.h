#ifndef __QCA7000_CONF__
#define __QCA7000_CONF__

const int SPI_SS_PIN = 10;
const int SPI_INT_PIN = 2;

//12MHz es el maximo de velocidad de SPI permitida - hoja 3 SPI QCA7000 protocol
//MSB - hoja 4 SPI QCA7000 protocol
//CPOL=1 && CPHA=1 modo SPI -> 3 - hoja 3 SPI QCA7000 protocol
#define QCA_SPI_CLK_SPEED  12000000
#define QCA_SPI_BIT_ORDER  MSBFIRST
#define QCA_SPI_MODE       SPI_MODE3

#define QCA_SPI_INT_CONFIG 0b00000001
#define QCA_SPI_INT_CLEAR  0b00000000

#ifndef DEBUG
#define DEBUG 1
#endif

#define UIP_LLADDR_LEN 6

#endif
