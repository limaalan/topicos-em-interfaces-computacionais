//PLACA MINHA
//Bibliotecas
#include <SPI.h>
#include "printf.h"
#include "RF24.h"
//Pinos usados
#define CE_PIN 7
#define CSN_PIN 8
//Definição dos tipos de mensagem
#define MSG 0
#define ACK 1
#define RTS 2
#define CTS 3
//Definição de variáveis
#define TIMEOUT 1000 // Tempo de espera pelo ACK
#define CS_TIME 70 // Carrier Sense Time, Milliseconds
#define CS_TIMEOUT 270 // Tempo de espera pra refazer o carrier sense
#define ID_REDE 77
#define CHANNEL 15

//Instanciando classes das bibliotecas de controle
RF24 radio(CE_PIN, CSN_PIN);
//Pipe de leitura e escrita ( deve ser o mesmo, não usar funções da biblioteca)
uint64_t address[2] = { 0x3030303030LL, 0x3030303030LL};
uint8_t meu_end=98;
uint8_t end_coordenador = 99 ; // Endereço do coordenador ( destino dos pacotes)

struct Payload {
  uint8_t id_rede = ID_REDE ; 
  uint8_t destino = end_coordenador ;
  uint8_t origem = meu_end;
  uint8_t tipo;
  uint8_t temperatura;
  uint8_t humidade;
};
Payload payloadRx ; // Estrutura para recebimento
Payload payload ; // Estrutura para transmissão

//mock dados
int temperatura_fake = 20;
int humidade_fake = 50;

void setup() {
  Serial.begin(115200);
  while (!Serial) {
  }

  // Inicializa módulo de radio
  if (!radio.begin()) {
    while (1) {
      Serial.println(F("radio hardware is not responding!!"));
      delay (500);
    } 
  }

  radio.setPALevel(RF24_PA_MAX);  
  radio.setChannel(CHANNEL);
  radio.setPayloadSize(sizeof(payload));
  radio.setAutoAck(false);
  radio.setCRCLength(RF24_CRC_DISABLED);
  radio.setDataRate(RF24_2MBPS);

  radio.openWritingPipe(address[0]); // Escrita no pipe 0
  radio.openReadingPipe(1, address[1]); // Leitura no pipe 1

  printf_begin(); // Print detalhes do hardware
  radio.printDetails(); // (smaller) function that prints raw register values
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
 
void sendPacket(Payload *payload, uint8_t tamanho, uint8_t destino, uint8_t controle){
    payload->tipo = controle;
    payload->destino = destino;
    //printPacote(payload);
    while(1){
       //Faz o Carrier Sense 
       radio.startListening();
       delayMicroseconds(CS_TIME);
       radio.stopListening();
       //Caso o meio estiver livre, envia
       if (!radio.testCarrier()) { 
          radio.write(payload, tamanho);
          return;
          
       }else{ //Caso contrario, espera
        Serial.println("Meio Ocupado");
        delayMicroseconds(CS_TIMEOUT);
       }
       radio.flush_rx();
    }
}

void loop() {
  payload.temperatura = (uint8_t)temperatura_fake;
  payload.humidade = (uint8_t)humidade_fake;

  byte bytes = radio.getPayloadSize();
  printPacote(&payload);
  sendPacket(&payload, bytes, end_coordenador, RTS);  // Transmite um pacote RTS, não há necessidade de salvar o resulado pois será sempre TRUE ( não usamos autoack)

  bool report = aguardaMsg(CTS); // Aguarda pelo CTS
  if(report){
    //Recebeu o CTS, agora envia o dado    
    sendPacket(&payload, bytes, end_coordenador, MSG); 
    report = aguardaMsg(ACK); // Aguarda ACK do dado enviado
  }    
  if(report){ // Recebeu ACK do dado enviado
    Serial.println("Sucesso!");
  }else{ // Não recebeu ACK do dado enviado
    Serial.println("FALHA!");
  }
  
  radio.flush_rx();
  delay(2000);

  // Gerando dados da temperatura.
  if(temperatura_fake>40){
    temperatura_fake = 20;
    humidade_fake = 20;
  }
  temperatura_fake+=1;
  humidade_fake+=2;
}
