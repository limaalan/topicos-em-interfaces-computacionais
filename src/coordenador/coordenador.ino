// PLACA PROFESSOR
#include <SPI.h>
#include "printf.h"
#include "RF24.h"

#define CE_PIN 7
#define CSN_PIN 8

#define MSG 0
#define ACK 1
#define RTS 2
#define CTS 3
#define TIMEOUT 1000 // Tempo de espera pelo pacote
#define CS_TIME 70 // Tempo do carrier sense, em microssegundos 

RF24 radio(CE_PIN, CSN_PIN);
uint64_t address[2] = { 0x3030303030LL, 0x3030303030LL};

//byte payload[6] = {0,1,2,3,4,5};
//byte payloadRx[6] = "     ";
uint8_t meu_end=99;

struct Payload {
  uint8_t id_rede = 99 ; 
  uint8_t destino ;
  uint8_t origem = meu_end;
  uint8_t tipo;
  //byte indice;
  uint8_t temperatura;
  uint8_t humidade;
};
Payload payloadRx ; // Estrutura para recebimento
Payload payload ; // Estrutura para transmissão

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    // some boards need to wait to ensure access to serial over USB
  }

  // initialize the transceiver on the SPI bus
  if (!radio.begin()) {
    while (1) {
    delay (500);
    Serial.println(F("radio hardware is not responding!!"));
    }  // hold in infinite loop
  }

  radio.setPALevel(RF24_PA_MAX);  // RF24_PA_MAX is default.
  radio.setChannel(110);
  radio.setPayloadSize(sizeof(payload));//sizeof(payload_recebimento));  // float datatype occupies 4 bytes
  radio.setAutoAck(false);
  //radio.disableDynamicPayloads();
  radio.setCRCLength(RF24_CRC_DISABLED);
  radio.setDataRate(RF24_2MBPS);
  radio.openWritingPipe(address[0]);  // always uses pipe 0
  radio.openReadingPipe(1, address[1]);  // using pipe 1

  //For debugging info
  printf_begin();             // needed only once for printing details
  radio.printDetails();       // (smaller) function that prints raw register values
  //radio.printPrettyDetails(); // (larger) function that prints human readable data

}

void printPacote(Payload *payload){
      Serial.print(F("Pacote de tamanho: "));
      Serial.print(radio.getPayloadSize());
      Serial.print(F(" Bytes | Dados -> Rede: "));
      Serial.print(payload->id_rede);
      Serial.print(F(" Dest: "));
      Serial.print(payload->destino);
      Serial.print(F(" Orig : "));
      Serial.print(payload->origem);
      Serial.print(F(" Tipo : "));
      Serial.print(payload->tipo);
      Serial.print(F(" Temp : "));
      Serial.print(payload->temperatura);
      Serial.print(F(" Hum : "));
      Serial.print(payload->humidade);
      Serial.println(); 
}


//Aguarda por TIMEOUT milisegundos uma mesangem do tipo TIPO
bool aguardaMsg(uint8_t tipo){
    uint8_t bytes = radio.getPayloadSize();  
    radio.startListening();
    unsigned long tempoInicio = millis();
    while(millis()-tempoInicio<TIMEOUT){
      if (radio.available()) {
        radio.read(&payloadRx, bytes);             
        if(payloadRx.destino==meu_end && payloadRx.tipo==tipo){
          radio.stopListening();
          return true;
        }
      }
      radio.flush_rx();
      delay(10);      
    }
    radio.stopListening();
    return false;
}
  

bool sendPacket(Payload *payload, uint8_t tamanho, uint8_t destino, uint8_t controle){
    payload->tipo = controle;
    payload->destino = destino;
   
    while(1){
       //Faz o Carrier Sense 
       radio.startListening();
       delayMicroseconds(CS_TIME);
       radio.stopListening();
       //Caso o meio estiver livre, envia
       if (!radio.testCarrier()) { 
          return radio.write(payload, tamanho);
          
       }else{ //Caso contrario, espera
        //Serial.println("Meio Ocupado");
        delayMicroseconds(270);
       }
       radio.flush_rx();
    }
}

void loop() {
  //Aguardando RTS
    radio.startListening();
    if (radio.available()) {// Recebeu algo
      uint8_t bytes = radio.getPayloadSize();// Obtém o tamanho do payload
      radio.read(&payloadRx, bytes );
      printPacote(&payloadRx);

      if(payloadRx.destino == meu_end && payloadRx.tipo==RTS){ // Pacote para mim do tipo RTS
        Serial.println("pacote pra mim : ");
        sendPacket(&payload, bytes, payloadRx.origem, CTS); // Responde um CTS com destino à origem do pacote recebido.
        bool report = aguardaMsg(MSG); // Aguarda dados
        if (report){
          sendPacket(&payload, bytes, payloadRx.origem, ACK); // Responde ACK para os dados
          Serial.println(F("ACK enviado: "));
          //printPacote(payload);
        }
      }
    } 
    //radio.stopListening();
    radio.flush_rx();
    delay(10);
}
