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

byte payload[5] = {0,1,2,3,4};
byte payloadRx[5] = "    ";
uint8_t origem=99;

struct Payload {
  byte id_rede = 99 ; 
  byte destino ;
  byte origem = origem;
  byte tipo;
  //byte indice;
  int temperatura;
  int humidade;
};
Payload payload_recebimento ;
Payload payload_transmissao ;


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

  radio.setPALevel(RF24_PA_MAX);  // RF24_PA_MAX is default.
  radio.setChannel(110);
  radio.setPayloadSize(12);//sizeof(payload_recebimento));  // float datatype occupies 4 bytes
  radio.setAutoAck(false);
  radio.setCRCLength(RF24_CRC_DISABLED);
  radio.setDataRate(RF24_1MBPS);

  radio.openWritingPipe(address[0]);  // always uses pipe 0
  radio.openReadingPipe(1, address[1]);  // using pipe 1

  //For debugging info
  printf_begin();             // needed only once for printing details
  radio.printDetails();       // (smaller) function that prints raw register values
  //radio.printPrettyDetails(); // (larger) function that prints human readable data

}

void printPacote(Payload *payload){
      Serial.print(F("Rede: "));
      Serial.print(payload->id_rede);
      Serial.print(F(" Destino: "));
      Serial.print(payload->destino);
      Serial.print(F(" Origem : "));
      Serial.print(payload->origem);
      Serial.print(F(" Tipo : "));
      Serial.print(payload->tipo);
      Serial.print(F(" Temperatura : "));
      Serial.print(payload->temperatura);
      Serial.print(F(" Humidade : "));
      Serial.print(payload->humidade);
      Serial.println(); 
}


//Aguarda por TIMEOUT milisegundos uma mesangem do tipo TIPO
bool aguardaMsg(int tipo){
    radio.startListening();
    unsigned long tempoInicio = millis();
    while(millis()-tempoInicio<TIMEOUT){
      if (radio.available()) {
        uint8_t bytes = radio.getPayloadSize();  
        radio.read(&payload_recebimento, bytes);             
        if(payload_recebimento.destino==origem && payload_recebimento.tipo==tipo){
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
  

bool sendPacket(Payload *payload_transmissao, int tamanho, int destino, int controle){
    payload_transmissao->tipo = controle;
    payload_transmissao->destino = destino;
   
    while(1){
       //Faz o Carrier Sense 
       radio.startListening();
       delayMicroseconds(CS_TIME);
       radio.stopListening();
       //Caso o meio estiver livre, envia
       if (!radio.testCarrier()) { 
          return radio.write(&payload_transmissao, tamanho);
          
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
      radio.read(&payload_recebimento, bytes);
      printPacote(&payload_recebimento);
      Serial.println(bytes);
      //Serial.println(payload_recebimento.id_rede);
      //Serial.println(payload_recebimento.origem);
      //Serial.println(payload_recebimento.destino);
      //Serial.println(payload_recebimento.temperatura);
      //Serial.println(payload_recebimento.humidade);       
      if(payload_recebimento.destino == origem && payload_recebimento.tipo==RTS){ // Pacote para mim do tipo RTS
        Serial.println("pacote pra mim : ");
        printPacote(&payload_recebimento);
        bool report = sendPacket(&payload_transmissao, sizeof(Payload), payload_recebimento.origem, CTS); // Responde um CTS com destino à origem do pacote recebido.
        //radio.startListening(); // Já tem no aguardaMsg
        report = aguardaMsg(MSG); // Aguarda dados
        if (report){
          bool report = sendPacket(&payload_transmissao, sizeof(Payload), payload_recebimento.origem, ACK); // Responde ACK para os dados
          Serial.print(F("ENVIADO: "));
          printPacote(&payload_transmissao);
        }
  
      }
      //printPacote(&payloadRx[0], bytes);
    }
    radio.flush_rx();
    delay(10);


}
