# Projeto de topicos em interfaces computacionais

## Objetivo:
Ulilizando um nó (uma placa) como gateway e 2 nós que trocam dados entre si, com um protoloco que se adeque à aplicação a ser utilizada. A escolha da aplicação fica a critério da equipe.
As informações de coletadas dos nós ou possibilidade de envio de informação à eles, o acesso aos dados e o controle nos nós (dependendo da aplicação) devem ser apresentadas em um smartphone por meio de app ou página web.
O resultado será uma aplicação IoT, onde os dados coletados dos nós serão acessados pelo usuário, ou o usuário poderá acionar algo conectado aos nós.

## Avaliação:
1. A equipe apresentará seu trabalho em um canal sem interferências.
2. O cana é alterado para um com outras equipes trocando msgs.
3. É adicionado ruido ao canal utilizado.
4. É utilizado um canal comum à uma rede wifi.

## Estrutura do projeto :
```
├── README.md 
├── src
│   ├── coordenador
│   │   └── coordenador.ino
│   └── no
│       └── no.ino
└── util
    ├── ALOHA
    ├── aloha2
    ├── CSMA com confirmação
    ├── struct
    └── versao_lista
```
* coordenador.ino : Código para a placa que cumprirá o papel de coordenador
* no.ino : Código para as placas que cumprirão o papel de nós da rede de sensores. 
* /util : Snippets de código úteis durante o desenvolvimento.
## Links Úteis : 
 - [Trabalhando com NRF24 + ethernet shield](https://forum.arduino.cc/t/nrf24l01-radios-rf24-library-ethernet-shield-do-not-work-together/195013/24)
 - [Tutoriais NRF24L01](https://forum.arduino.cc/t/simple-nrf24l01-2-4ghz-transceiver-demo/405123/64)
 - [Documentação da biblioteca NRF24](https://nrf24.github.io/RF24/)
 - [Módulo de rádio para quando se usa um servo motor](https://forum.arduino.cc/t/when-i-attach-the-servo-nrf24-confused-it-no-longer-runs-the-motors/1059329/7)