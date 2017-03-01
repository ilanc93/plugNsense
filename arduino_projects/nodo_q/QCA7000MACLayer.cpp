#include "QCA7000MACLayer.h"
#include "MACLayer.h"
#include "QCA7000_commons.h"
#include "QCA7000_conf.h"
#include <Arduino.h>
#include <SPI.h>


QCA7000MACLayer::QCA7000MACLayer(const uint8_t mac[UIP_LLADDR_LEN], uint8_t *myBuffer){
  memcpy(&my_mac, mac, UIP_LLADDR_LEN);
  buffer = myBuffer;
}


bool QCA7000MACLayer::init(){
  pinMode(SPI_SS_PIN, OUTPUT);
  digitalWrite(SPI_SS_PIN, HIGH);
  pinMode(SPI_INT_PIN, INPUT);
  
  SPI.begin();
  
  //Initializes SPI with the defined settings.
  SPI.beginTransaction(SPISettings(QCA_SPI_CLK_SPEED, QCA_SPI_BIT_ORDER, QCA_SPI_MODE));
  SPI.endTransaction();
  digitalWrite(SPI_SS_PIN, HIGH);
  //Avoid SPI bug writing an extra bit.
  SPI.transfer16(QCA_SPI_REG_SIGNATURE);
  
  const uint16_t qca_spi_receive_command = qca_spi_readRegister( QCA_SPI_REG_SIGNATURE );
  if ( qca_spi_receive_command != QCA_SPI_REG_SIGNATURE_VALUE ) {
    /*
     Serial.println("FALLO SPI");
    Serial.print("Comando leido: ");
    Serial.println(qca_spi_receive_command, HEX);
     */
    return false;
  }
  
  /*Serial.println("SPI CONECTADO");*/
  
  //Clear interrupt flags
  qca_spi_writeRegister(QCA_SPI_REG_INTR_CAUSE, 0);
  
  //Enable configured interrupts.
  qca_spi_writeRegister(QCA_SPI_REG_INTR_ENABLE, QCA_SPI_INT_CONFIG);
  
  
  //Attach interrupt to handler.
  attach_Interrupt();

  return true;
}

MACTransmissionStatus QCA7000MACLayer::send(const uint8_t* mac_destino, uint8_t* data, uint16_t length){

  
  
  //Auxiliary variable to use in for loops
  uint16_t i;
  
  //QCA_SPI_REG_BFR_SIZE = SOF + FL + RSVD + ETH + EOF;
  const uint16_t bufferSize = QCA_SPI_SOF_LEN + QCA_SPI_FL_LEN +
  QCA_SPI_RSVD_LEN + QCA_SPI_EOF_LEN + length + ETHERNET_HEADER_LENGTH;
  
  const uint16_t writeBufferAvailable = qca_spi_readRegister( QCA_SPI_REG_WRBUF_SPC_AVA );
  
  if ( writeBufferAvailable < bufferSize ) {
    return MAC_TX_STATUS_ERR;
  }
  
  qca_spi_writeRegister( QCA_SPI_REG_BFR_SIZE, bufferSize );
  digitalWrite(SPI_SS_PIN, LOW);
  qca_spi_startExternalWrite();
  
  /*
   Escribir los datos. El orden a mandar los datos es:
   1 - SOF
   2 - FL
   3 - RSVD
   4 - ETH
   5 - EOF
  */
  
  
  //Start of Frame
  Serial.println("SOF");
  for ( i = 0; i < QCA_SPI_SOF_LEN; i++ ) {
    SPI.transfer( QCA_SPI_READ_CHECK_SOF[i] );
    Serial.print( QCA_SPI_READ_CHECK_SOF[i] , HEX);
    Serial.print(" | ");
  }
  Serial.println("");
  
  myUInt16_t frameLength;
  frameLength.value = length + ETHERNET_HEADER_LENGTH;//We add 8 uint8_ts per mac and 2 for ethertype
  //Frame Length en Little Endian (Primero el uint8_t mas significativo)
  SPI.transfer(frameLength.valueArray[0]);
  SPI.transfer(frameLength.valueArray[1]);
  
  
  
  Serial.println("FL");
  Serial.print(frameLength.valueArray[0], HEX);
  Serial.print(frameLength.valueArray[1], HEX);
  Serial.print(" o ");
  Serial.print(frameLength.value, DEC);
  Serial.println("");
  
  //Reserved
  Serial.println("RSVD");
  for ( i = 0; i < QCA_SPI_RSVD_LEN; i++ ) {
    SPI.transfer( QCA_SPI_READ_CHECK_RSVD[i] );
    Serial.print( QCA_SPI_READ_CHECK_RSVD[i], HEX);
    Serial.print( " | ");
  }
  Serial.println("");
  
  //Send destination MAC
  Serial.println("MAC destino");
  
  for( i = 0; i < UIP_LLADDR_LEN; i++){
    SPI.transfer(mac_destino[i]);
    Serial.print( mac_destino[i], HEX);
  }
  Serial.println("");
  
  Serial.println("MAC origen");
  //Send source MAC
  for ( i = 0; i < UIP_LLADDR_LEN; i++) {
    SPI.transfer(my_mac[i]);
    Serial.print(my_mac[i], HEX);
  }
  Serial.println("");
  
  //Send Ethertype.
  SPI.transfer(ETHERNET_ETHERTYPE_HIGH);
  SPI.transfer(ETHERNET_ETHERTYPE_LOW);
  Serial.println("Ethertype");
  Serial.print(ETHERNET_ETHERTYPE_HIGH, HEX);
  Serial.print(ETHERNET_ETHERTYPE_LOW + 1, HEX);
  Serial.println("");
  
  //Ethernet Payload
  for ( i = 0; i < length; i++) {
    SPI.transfer( data[i] );
  }
  
  //End of Frame
  for ( i = 0; i < QCA_SPI_EOF_LEN; i++ ) {
    SPI.transfer( QCA_SPI_READ_CHECK_EOF[i] );
  }
  
  //We have finished writing, set CS back to HIGH.
  digitalWrite(SPI_SS_PIN, HIGH);

  
  
#if DEBUG
  Serial.println("Sent packet: ");
  for ( i=0; i < length; i++) {
    Serial.print(data[i], HEX);
    Serial.print(" | ");
    if ( i%10 == 9 ){
      Serial.println("");
    }
  }
  Serial.println("");
#endif

    return MAC_TX_STATUS_OK;

}

bool QCA7000MACLayer::receive(uint8_t* lladdr_src, uint8_t* lladdr_dest, uint8_t* data, uint16_t* length){
  if ( !frameToRead ){
    return false;
  }else{
    frameToRead = false;
    memcpy(lladdr_src, src_mac_buffer, UIP_LLADDR_LEN);
    memcpy(lladdr_dest, dest_mac_buffer, UIP_LLADDR_LEN);
    *length = bufferLen;
    return true;
  }
  
  /*
  detachInterrupt(digitalPinToInterrupt(SPI_INT_PIN));
  
  if ( !huboInterrupt ){
    attach_Interrupt();
    return false;
  }
  
  huboInterrupt = packetAvailable();
  
  if ( !huboInterrupt ){
    attach_Interrupt();
    return false;
  }
  
  huboInterrupt = false;
  
  
  bool huboError = false;
  
  uint16_t i;
  
  
  
  //Lee la cantidad de datos que hay en el buffer para leer.
  uint16_t readBufferAvailable = qca_spi_readRegister( QCA_SPI_REG_RDBUF_BYTE_AVA );
  
  
  if ( readBufferAvailable > (ETHERNET_MAX_FRAME_SIZE + ETHERNET_HEADER_LENGTH) ) {
    readBufferAvailable = ETHERNET_MAX_FRAME_SIZE + ETHERNET_HEADER_LENGTH;
  }
  */
  
  //qca_spi_writeRegister( QCA_SPI_REG_BFR_SIZE, readBufferAvailable );
  //digitalWrite(SPI_SS_PIN, LOW);
  //qca_spi_startExternalRead();
  /*
   Leer los datos. El orden a leer de los datos es:
   1 - LEN
   2 - SOF
   3 - FL
   4 - RSVD
   5 - ETH
   6 - EOF
    */
  /*
  //Read Packet length
  SPI.transfer16( 0 );
  readBufferAvailable = SPI.transfer16( 0 );
  
  //Read SOF.
  for ( i = 0; i < QCA_SPI_SOF_LEN; i++ ) {
    if ( QCA_SPI_READ_CHECK_SOF[i] != SPI.transfer( 0 ) ) {
      huboError = true;
    }
  }
  readBufferAvailable -= QCA_SPI_SOF_LEN;
  
  //Si fallo la lectura del SOF limpia el buffer.
  if ( huboError ) {
    for ( i = 0; i < readBufferAvailable; i++ ) {
      SPI.transfer( 0 );
    }
    digitalWrite(SPI_SS_PIN, HIGH);
    attach_Interrupt();
    return false;
  }
  
  //Read FL en Little Endian
  myUInt16_t frameLength;
  frameLength.valueArray[0] = SPI.transfer( 0 );
  frameLength.valueArray[1] = SPI.transfer( 0 );
  readBufferAvailable -= QCA_SPI_FL_LEN;
  
  //Reserved
  for ( i = 0; i < QCA_SPI_RSVD_LEN; i++ ) {
    if ( QCA_SPI_READ_CHECK_RSVD[i] != SPI.transfer( 0 ) ) {
      huboError = true;
    }
  }
  readBufferAvailable -= QCA_SPI_RSVD_LEN;
  
  if ( huboError ) {
    for ( i = 0; i < readBufferAvailable; i++ ) {
      SPI.transfer( 0 );
    }
    digitalWrite(SPI_SS_PIN, HIGH);
    attach_Interrupt();
    return false;
  }
  
  
  //Read source mac (destination in transmit frame)
  for ( i = 0; i < UIP_LLADDR_LEN; i++) {
    lladdr_src[i] = SPI.transfer( 0 );
  }
  
  for ( i = 0; i < UIP_LLADDR_LEN; i++) {
    lladdr_dest[i] = SPI.transfer( 0 );
  }
  
  //Read EtherType
  SPI.transfer( 0 );
  SPI.transfer( 0 );
  
  
  *length = frameLength.value - ETHERNET_HEADER_LENGTH;
  //Ethernet Frame - INCLUDING MAC_DEST, MAC_SRC and ETHERTYPE.
  for ( i = 0; i < (*length); i++) {
    data[i] = SPI.transfer( 0 );
  }
  
  //End of Frame
  for ( i = 0; i < QCA_SPI_EOF_LEN; i++ ) {
    if ( QCA_SPI_READ_CHECK_EOF[i] != SPI.transfer( 0 ) ) {
      huboError = true;
    }
  }
  
  if ( huboError ) {
    digitalWrite(SPI_SS_PIN, HIGH);
    attach_Interrupt();
    return false;
  }
  
  //Terminada la lectura, levanta el pin de chip select.
  digitalWrite(SPI_SS_PIN, HIGH);

  
#if DEBUG
  Serial.println("Received packet: ");
  for ( i=0; i < *length; i++) {
    Serial.print(data[i], HEX);
    Serial.print(" | ");
    if ( i%10 == 9 ){
      Serial.println("");
    }
  }
  Serial.println("");
#endif
  
  
  delay(1);
  attach_Interrupt();
  //huboInterrupt = false;
  return true;
   */
}

const uint8_t* QCA7000MACLayer::getMacAddress(){
  return my_mac;
}




void QCA7000MACLayer::interruptHandler(){
  //detachInterrupt(digitalPinToInterrupt(SPI_INT_PIN));
  noInterrupts();

  Serial.println("Hubo interrupt");
  if ( !frameToRead ){
    frameToRead = readParseFrame();
    Serial.println(frameToRead == true ? "True" : "False");
  }
  interrupts();
  //attach_Interrupt();
}




bool QCA7000MACLayer::readParseFrame(){
  if ( !packetAvailable() ){
    return false;
  }
  
  bool huboError = false;
  uint16_t i;
  
  //Lee la cantidad de datos que hay en el buffer para leer.
  uint16_t readBufferAvailable = qca_spi_readRegister( QCA_SPI_REG_RDBUF_BYTE_AVA );
  
  if ( readBufferAvailable > (ETHERNET_MAX_FRAME_SIZE + ETHERNET_HEADER_LENGTH) ) {
    readBufferAvailable = ETHERNET_MAX_FRAME_SIZE + ETHERNET_HEADER_LENGTH;
  }
  
  qca_spi_writeRegister( QCA_SPI_REG_BFR_SIZE, readBufferAvailable );
  digitalWrite(SPI_SS_PIN, LOW);
  qca_spi_startExternalRead();
  /*
   Leer los datos. El orden a leer de los datos es:
   1 - LEN
   2 - SOF
   3 - FL
   4 - RSVD
   5 - ETH
   6 - EOF
   */
  
  //Read Packet length
  SPI.transfer16( 0 );
  readBufferAvailable = SPI.transfer16( 0 );
  
  //Read SOF.
  for ( i = 0; i < QCA_SPI_SOF_LEN; i++ ) {
    if ( QCA_SPI_READ_CHECK_SOF[i] != SPI.transfer( 0 ) ) {
      huboError = true;
    }
  }
  readBufferAvailable -= QCA_SPI_SOF_LEN;
  
  //Si fallo la lectura del SOF limpia el buffer.
  if ( huboError ) {
    for ( i = 0; i < readBufferAvailable; i++ ) {
      SPI.transfer( 0 );
    }
    digitalWrite(SPI_SS_PIN, HIGH);
    attach_Interrupt();
    return false;
  }
  
  //Read FL en Little Endian
  myUInt16_t frameLength;
  frameLength.valueArray[0] = SPI.transfer( 0 );
  frameLength.valueArray[1] = SPI.transfer( 0 );
  readBufferAvailable -= QCA_SPI_FL_LEN;
  
  //Reserved
  for ( i = 0; i < QCA_SPI_RSVD_LEN; i++ ) {
    if ( QCA_SPI_READ_CHECK_RSVD[i] != SPI.transfer( 0 ) ) {
      huboError = true;
    }
  }
  readBufferAvailable -= QCA_SPI_RSVD_LEN;
  
  if ( huboError ) {
    for ( i = 0; i < readBufferAvailable; i++ ) {
      SPI.transfer( 0 );
    }
    digitalWrite(SPI_SS_PIN, HIGH);
    attach_Interrupt();
    return false;
  }
  
  
  //Read destination mac
  for ( i = 0; i < UIP_LLADDR_LEN; i++) {
    dest_mac_buffer[i] = SPI.transfer( 0 );
  }
  //Read source mac
  for ( i = 0; i < UIP_LLADDR_LEN; i++) {
    src_mac_buffer[i] = SPI.transfer( 0 );
  }
  
  //Read EtherType
  SPI.transfer( 0 );
  SPI.transfer( 0 );
  
  
  bufferLen = frameLength.value - ETHERNET_HEADER_LENGTH;
  //Ethernet Frame - INCLUDING MAC_DEST, MAC_SRC and ETHERTYPE.
  for ( i = 0; i < bufferLen; i++) {
    buffer[i] = SPI.transfer( 0 );
  }
  
  //End of Frame
  for ( i = 0; i < QCA_SPI_EOF_LEN; i++ ) {
    if ( QCA_SPI_READ_CHECK_EOF[i] != SPI.transfer( 0 ) ) {
      huboError = true;
    }
  }
  
  if ( huboError ) {
    digitalWrite(SPI_SS_PIN, HIGH);
    return false;
  }
  
  //Terminada la lectura, levanta el pin de chip select.
  digitalWrite(SPI_SS_PIN, HIGH);
  return true;
}




uint16_t QCA7000MACLayer::qca_spi_readRegister(const uint16_t spi_register) {
  uint16_t qca_spi_send_command = 0;
  qca_spi_send_command |= ( 1 << 15 );
  qca_spi_send_command |=  1 << 14;
  qca_spi_send_command |=  spi_register & 0x1FFF;
  
  //Serial.print("Comando enviado: ");
  //Serial.println(qca_spi_send_command, HEX);
  
  
  digitalWrite(SPI_SS_PIN, LOW);
  uint16_t readResponse = SPI.transfer16(qca_spi_send_command);
  readResponse = SPI.transfer16(qca_spi_send_command);
  //  uint16_t aux = ((readResponse << 8) & 0xFF00) | ((readResponse >> 8) & 0xFF);
  //  readResponse = aux;
  digitalWrite(SPI_SS_PIN, HIGH);
  
  return readResponse;
}


uint16_t QCA7000MACLayer::qca_spi_writeRegister(const uint16_t spi_register, const uint16_t register_value) {
  uint16_t qca_spi_send_command = 0;
  qca_spi_send_command |=  1 << 14;
  qca_spi_send_command |=  spi_register & 0x1FFF;
  
  digitalWrite(SPI_SS_PIN, LOW);
  uint16_t readResponse = SPI.transfer16(qca_spi_send_command);
  readResponse = SPI.transfer16(register_value);
  //  uint16_t aux = ((readResponse << 8) & 0xFF00) | ((readResponse >> 8) & 0xFF);
  //  readResponse = aux;
  digitalWrite(SPI_SS_PIN, HIGH);
  
  return readResponse;
}


inline void QCA7000MACLayer::qca_spi_startExternalWrite() {
  SPI.transfer16( QCA_SPI_CONF_REG_EXTERNAL_WRITE );
}

inline void QCA7000MACLayer::qca_spi_startExternalRead() {
  SPI.transfer16( QCA_SPI_CONF_REG_EXTERNAL_READ );
}


uint8_t* QCA7000MACLayer::getBuffer(){
  return buffer;
}

bool QCA7000MACLayer::packetAvailable(){
  bool available = false;
  
  //Deshabilita las interrupciones
  qca_spi_writeRegister(QCA_SPI_REG_INTR_ENABLE, QCA_SPI_INT_CLEAR);
  
  //Lee el motivo
  uint16_t readResponse = qca_spi_readRegister(QCA_SPI_REG_INTR_CAUSE);
  
  //Hay un paquete disponible
  if ( CHECK_BIT(readResponse, 0) ) {
    available = true;
  }
  
  
  //Limpia las flags de interrupciones - se confirma el motivo
  qca_spi_writeRegister(QCA_SPI_REG_INTR_CAUSE, readResponse);
  
  //Rehabilita las interrupciones
  qca_spi_writeRegister(QCA_SPI_REG_INTR_ENABLE, QCA_SPI_INT_CONFIG);
  return available;
}


inline void QCA7000MACLayer::attach_Interrupt(){
  attachInterrupt(digitalPinToInterrupt(SPI_INT_PIN), qca_interrupt_handler, RISING);
}

