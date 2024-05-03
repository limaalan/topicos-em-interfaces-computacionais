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
byte payload[5] = {0,1,2,3,4};
byte payloadRx[5] = "    ";
uint8_t origem=98;
uint8_t end_coordenador = 99 ; // Endereço do coordenador ( destino dos pacotes)
uint8_t indice=0;


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
  radio.setPayloadSize(sizeof(payload));  // float datatype occupies 4 bytes
  radio.setAutoAck(false);
  radio.setCRCLength(RF24_CRC_DISABLED);
  radio.setDataRate(RF24_2MBPS);

  radio.openWritingPipe(address[0]);  // always uses pipe 0
  radio.openReadingPipe(1, address[1]);  // using pipe 1

  //For debugging info
  printf_begin();             // needed only once for printing details
  radio.printDetails();       // (smaller) function that prints raw register values
  //radio.printPrettyDetails(); // (larger) function that prints human readable data


}

void printPacote(byte *pac, int tamanho){
      Serial.print(F("Rcvd "));
      Serial.print(tamanho);  // print the size of the payload
      Serial.print(F(" O: "));
      Serial.print(pac[0]);  // print the payload's value
      Serial.print(F(" D: "));
      Serial.print(pac[1]);  // print the payload's value
      Serial.print(F(" C: "));
      Serial.print(pac[2]);  // print the payload's value
      Serial.print(F(" i: "));
      Serial.print(pac[3]);  // print the payload's value
      Serial.print(F(" : "));
      for(int i=4;i<tamanho;i++){
        Serial.print(pac[i]);
      }
      Serial.println();  // print the payload's value
}
//Aguarda por TIMEOUT milisegundos uma mesangem do tipo TIPO
bool aguardaMsg(int tipo){
    radio.startListening();
    unsigned long tempoInicio = millis();
    while(millis()-tempoInicio<TIMEOUT){
      if (radio.available()) {
        uint8_t bytes = radio.getPayloadSize();  // get the size of the payload
        radio.read(&payloadRx[0], bytes);             // fetch payload from FIFO
        if(payloadRx[1]==origem && payloadRx[2]==tipo){
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
 
bool sendPacket(byte *pacote, int tamanho, int destino, int controle){
    pacote[0]=origem;
    pacote[1]=destino;
    pacote[2]=controle;
    pacote[3]=indice;
    //for(int i=0;i<tamanho;i++){
    //  Serial.print(pacote[i]);
    //}
    //Serial.println();
   
    while(1){
       //Faz o Carrier Sense 
       radio.startListening();
       delayMicroseconds(CS_TIME);
       radio.stopListening();
       //Caso o meio estiver livre, envia
       if (!radio.testCarrier()) { 
          return radio.write(&pacote[0], tamanho);
          
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
    Serial.print("Temperature: ");
    Serial.print(temperatura);
    Serial.print(" °C\tHumidity: ");
    Serial.print(humidade);
    Serial.println(" %");
  } else {
    Serial.println(DHT11::getErrorString(result));
  }
  delay (1000);
  return 0 ;

  if (Serial.available()) {
    char c = toupper(Serial.read());
    if (c == 'T') {
      //unsigned long start_timer = micros();                // Começa o Timer
      bool report = sendPacket(&payload[0], sizeof(payload), end_coordenador, RTS);  // Transmite um pacote RTS e salva o resultado em report
      report = aguardaMsg(CTS); // Aguarda pelo CTS
      if(report){
        //Recebeu o CTS, agora envia o dado
        sendPacket(&payload[0], sizeof(payload), end_coordenador, MSG); 
        report = aguardaMsg(ACK); // Aguarda ACK do dado enviado
      }
      
      //unsigned long end_timer = micros();                  // end the timer
      if(report){ // Recebeu ACK do dado enviado
         Serial.println("Sucesso!");
      }else{ // Não recebeu ACK do dado enviado
         Serial.println("FALHA!");
      }

    }
  }

    radio.flush_rx();
    delay(10);


}
