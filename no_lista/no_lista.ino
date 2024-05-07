//PLACA MINHA
//Bibliotecas
#include <SPI.h>
#include "printf.h"
#include "RF24.h"
#include "DHT11.h"
//Pinos usados
#define CE_PIN 8
#define CSN_PIN 10
#define DHT11_PIN 2
//Definição dos tipos de mensagem
#define MSG 0
#define ACK 1
#define RTS 2
#define CTS 3
//Definição de variáveis
#define TIMEOUT 1000 // Tempo de espera pelo ACK
#define CS_TIME 70 // Carrier Sense Time, Milliseconds
//Instanciando classes das bibliotecas de controle
DHT11 dht11(DHT11_PIN);
RF24 radio(CE_PIN, CSN_PIN);
//Pipe de leitura e escrita ( deve ser o mesmo, não usar funções da biblioteca)
uint64_t address[2] = { 0x3030303030LL, 0x3030303030LL};
uint8_t meu_end=98;
uint8_t end_coordenador = 99 ; // Endereço do coordenador ( destino dos pacotes)

//Definindo variáveis e structs que serão utilizadas na transmissão
byte payload[6] = {99,1,meu_end,3,4,5};
byte payloadRx[6] = "      ";

//struct Payload {
//  uint8_t id_rede = 99 ; 
//  uint8_t destino = end_coordenador ;
//  uint8_t origem = meu_end;
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
    while (1) {
      Serial.println(F("radio hardware is not responding!!"));
      delay (2000);
    }  // hold in infinite loop
  }

  radio.setPALevel(RF24_PA_MAX);  // RF24_PA_MAX is default.
  radio.setChannel(110);
  radio.setPayloadSize(sizeof(payload));//sizeof(payload_transmissao));  // float datatype occupies 4 bytes
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
      Serial.print(F("Pacote de tamanho: "));
      Serial.print(radio.getPayloadSize());
      Serial.print(F(" Dados -> Rede: "));
      Serial.print(payload[0]);
      Serial.print(F(" Dest: "));
      Serial.print(payload[1]);
      Serial.print(F(" Orig : "));
      Serial.print(payload[2]);
      Serial.print(F(" Tipo : "));
      Serial.print(payload[3]);
      Serial.print(F(" Temp : "));
      Serial.print(payload[4]);
      Serial.print(F(" Hum : "));
      Serial.print(payload[5]);
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
        if(payloadRx[1]==meu_end && payloadRx[3]==tipo){
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
 
void sendPacket(byte *payload, uint8_t tamanho, uint8_t destino, uint8_t controle){
    payload[3] = controle;
    payload[1] = destino;
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
        delayMicroseconds(270);
       }
       radio.flush_rx();
    }
}


void loop() {
  
  int temperatura = 0 ;
  int humidade = 0 ;
  int result = dht11.readTemperatureHumidity ( temperatura, humidade ) ;
  if (result != 0 ){
    // Não leu corretamente
    Serial.println(DHT11::getErrorString(result));
    temperatura = -253 ; 
    humidade = - 1;
  }

  payload[4] = (uint8_t)temperatura;
  payload[5] = (uint8_t)humidade;

  byte bytes = radio.getPayloadSize();
  //printPacote(payload);
  //bool report = sendPacket(payload, bytes, end_coordenador, RTS);  // Transmite um pacote RTS e salva o resultado em report
  sendPacket(payload, bytes, end_coordenador, RTS);  // Transmite um pacote RTS, não há necessidade de salvar o resulado pois será sempre TRUE ( não usamos autoack)

  bool report = aguardaMsg(CTS); // Aguarda pelo CTS
  if(report){
    //Recebeu o CTS, agora envia o dado    
    sendPacket(payload, bytes, end_coordenador, MSG); 
    report = aguardaMsg(ACK); // Aguarda ACK do dado enviado
  }    
  if(report){ // Recebeu ACK do dado enviado
    Serial.println("Sucesso!");
  }else{ // Não recebeu ACK do dado enviado
    Serial.println("FALHA!");
  }
  
  radio.flush_rx();
  delay(2000);


}
