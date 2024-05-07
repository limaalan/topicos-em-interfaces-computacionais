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
#define CS_TIME 50 // Tempo do carrier sense, em microssegundos 

RF24 radio(CE_PIN, CSN_PIN);
uint64_t address[2] = { 0x3030303030LL, 0x3030303030LL};

byte payload[6] = {0,1,2,3,4,5};
byte payloadRx[6] = "     ";
uint8_t origem=99;

//struct Payload {
//  uint8_t id_rede = 99 ; 
//  uint8_t destino ;
//  uint8_t origem = origem;
//  uint8_t tipo;
  //byte indice;
//  int temperatura;
//  int humidade;
//};
//Payload payload_recebimento ;
//Payload payload_transmissao ;

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    // some boards need to wait to ensure access to serial over USB
  }

  // initialize the transceiver on the SPI bus
  if (!radio.begin()) {
    Serial.println(F("radio hardware is not responding!!"));
    while (1) {}  // hold in infinite loop
  }

  radio.setPALevel(RF24_PA_LOW);  // RF24_PA_MAX is default.
  radio.setChannel(100);
  radio.setPayloadSize(sizeof(payload));//sizeof(payload_recebimento));  // float datatype occupies 4 bytes
  radio.setAutoAck(false);
  radio.setCRCLength(RF24_CRC_DISABLED);
  radio.setDataRate(RF24_2MBPS);
  //radio.disableDynamicPayloads();


  radio.openWritingPipe(address[0]);  // always uses pipe 0
  radio.openReadingPipe(1, address[1]);  // using pipe 1

  //For debugging info
  printf_begin();             // needed only once for printing details
  radio.printDetails();       // (smaller) function that prints raw register values
  //radio.printPrettyDetails(); // (larger) function that prints human readable data

}

void printPacote(byte *payload){
      Serial.print(F("Rede: "));
      Serial.print(payload[0]);
      Serial.print(F(" Destino: "));
      Serial.print(payload[1]);
      Serial.print(F(" Origem : "));
      Serial.print(payload[2]);
      Serial.print(F(" Tipo : "));
      Serial.print(payload[3]);
      Serial.print(F(" Temperatura : "));
      Serial.print(payload[4]);
      Serial.print(F(" Humidade : "));
      Serial.print(payload[5]);
      Serial.println(); 
}


//Aguarda por TIMEOUT milisegundos uma mesangem do tipo TIPO
bool aguardaMsg(int tipo){
    radio.startListening();
    unsigned long tempoInicio = millis();
    while(millis()-tempoInicio<TIMEOUT){
      if (radio.available()) {
        uint8_t bytes = radio.getPayloadSize();  
        radio.read(&payloadRx, bytes);             
        if(payloadRx[1]==origem && payloadRx[3]==tipo){
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
  

bool sendPacket(byte *payload, int tamanho, int destino, int controle){
    payload[3] = controle;
    payload[1] = destino;
   
    while(1){
       //Faz o Carrier Sense 
       radio.startListening();
       delayMicroseconds(CS_TIME);
       radio.stopListening();
       //Caso o meio estiver livre, envia
       if (!radio.testCarrier()) { 
          return radio.write(&payload, tamanho);
          
       }else{ //Caso contrario, espera
        Serial.println("Meio Ocupado");
        delayMicroseconds(100);
       }
       radio.flush_rx();
    }
}

void loop() {
  //Aguardando RTS
    radio.startListening();
    if (radio.available()) {              // Recebeu algo
      uint8_t bytes = radio.getPayloadSize();  // Obtém o tamanho do payload
      radio.read(&payloadRx, bytes );

      //printPacote(payloadRx);
      Serial.println(bytes);
      Serial.println(payloadRx[0]);
      Serial.println(payloadRx[1]);
      Serial.println(payloadRx[2]);
      Serial.println(payloadRx[3]);
      Serial.println(payloadRx[4]);
      Serial.println(payloadRx[5]);
      Serial.println(F("--------"));
      //if(payload_recebimento.destino == origem && payload_recebimento.tipo==RTS){ // Pacote para mim do tipo RTS
      //  Serial.println("pacote pra mim : ");
      //  printPacote(&payload_recebimento);
      //  bool report = sendPacket(&payload_transmissao, 12, payload_recebimento.origem, CTS); // Responde um CTS com destino à origem do pacote recebido.
        //radio.startListening(); // Já tem no aguardaMsg
      //  report = aguardaMsg(MSG); // Aguarda dados
      //  if (report){
      //    bool report = sendPacket(&payload_transmissao, 12, payload_recebimento.origem, ACK); // Responde ACK para os dados
      //    Serial.print(F("ENVIADO: "));
      //    printPacote(&payload_transmissao);
      //  }
  
      //}
    }
    //radio.stopListening();
    radio.flush_rx();
    delay(10);


}
