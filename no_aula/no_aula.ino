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
#define CS_TIME 70 // Carrier Sense Time, Milliseconds
#define TIMEOUT 1000 // Tempo de espera pelo ACK
//Instanciando classes das bibliotecas de controle
DHT11 dht11(DHT11_PIN);
RF24 radio(CE_PIN, CSN_PIN);
//Pipe de leitura e escrita ( deve ser o mesmo, não usar funções da biblioteca)
uint64_t address[2] = { 0x3030303030LL, 0x3030303030LL};

//Definindo variáveis e structs que serão utilizadas na transmissão
//byte payload[5] = {0,1,2,3,4};
//byte payloadRx[5] = "    ";
uint8_t meu_end=98;
uint8_t end_coordenador = 99 ; // Endereço do coordenador ( destino dos pacotes)

struct Payload {
  uint8_t id_rede = 99 ; 
  uint8_t destino = end_coordenador ;
  uint8_t origem = meu_end;
  uint8_t tipo;
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
  radio.setPayloadSize(8);//sizeof(payload_transmissao));  // float datatype occupies 4 bytes
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
bool aguardaMsg(uint8_t tipo){
    radio.startListening();
    unsigned long tempoInicio = millis();
    while(millis()-tempoInicio<TIMEOUT){
      if (radio.available()) {
        uint8_t bytes = radio.getPayloadSize(); 
        radio.read(&payload_recebimento, bytes);             
        if(payload_recebimento.destino==meu_end && payload_recebimento.tipo==tipo){
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
 
bool sendPacket(Payload *payload_transmissao, int tamanho, uint8_t destino, uint8_t controle){
    payload_transmissao->tipo = controle;
    payload_transmissao->destino = destino;
    printPacote(payload_transmissao);
    while(1){
       //Faz o Carrier Sense 
       radio.startListening();
       delayMicroseconds(CS_TIME);
       radio.stopListening();
       //Caso o meio estiver livre, envia
       if (!radio.testCarrier()) { 
          uint8_t * byteArray  ;
          byteArray = (uint8_t *) &payload_transmissao ;  // treat struct as byte arraay

          return radio.write( &byteArray[ 0 ] , tamanho );
          //return radio.write(&payload_transmissao, tamanho);
          
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
  if (result == 0 ){
    Serial.print("Temperatura: ");
    Serial.print(temperatura);
    Serial.print(" °C\tHumidade: ");
    Serial.print(humidade);
    Serial.println(" %");
  } else {
    Serial.println(DHT11::getErrorString(result));
    temperatura = -253 ; // Erro na leitura 
    humidade = - 1;
  }

  //if (Serial.available()) {
    //char c = toupper(Serial.read());
    //if (c == 'T') {
      payload_transmissao.temperatura = temperatura;
      payload_transmissao.humidade = humidade;
      byte bytes = radio.getPayloadSize();
      Serial.println(bytes);

      bool report = sendPacket(&payload_transmissao, 8, end_coordenador, RTS);  // Transmite um pacote RTS e salva o resultado em report
      report = aguardaMsg(CTS); // Aguarda pelo CTS
      if(report){
        //Recebeu o CTS, agora envia o dado
        
        sendPacket(&payload_transmissao, 8, end_coordenador, MSG); 
        report = aguardaMsg(ACK); // Aguarda ACK do dado enviado
      }
      
      //unsigned long end_timer = micros();                  // end the timer
      if(report){ // Recebeu ACK do dado enviado
         Serial.println("Sucesso!");
      }else{ // Não recebeu ACK do dado enviado
         Serial.println("FALHA!");
      }

    //}
  //}

    radio.flush_rx();
    delay(1000);


}
