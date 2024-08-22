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
├─  topicos-em-interfaces-computacionais
│   README.md
└───src
    ├───Arduino
    │   ├───coordenador
    │   │       coordenador.ino
    │   │       
    │   ├───no
    │   │       no.ino
    │   │       
    │   └───no_dados_aleatorios
    │           no_dados_aleatorios.ino
    │           
    └───Servidor
        │   index.js
        │   package-lock.json
        │   package.json
        │   
        └───public
                index.html
```
* coordenador.ino : Código para a placa que receberá os dados.
* no.ino : Código para as placas que farão a leitura de temperatura. 
* no_dados_aleatorios.ino : Versão que não faz leitura real do sensor de temperatura, apenas manda um número qualquer. 
* /Servidor : Arquivos para executar um servidor web local que realiza a leitura da porta serial e mostra o resultado em uma página.

## Como executar o projeto 
Compile e carregue o código de cada placa.

No diretório **/Servidor** :
- Execute o comando `npm install` para instalar os módulos
- Altere a porta serial que será lida no arquivo `index.js`, e execute o comando `node index.js` para iniciar o servidor.

Veja o resultado das leituras na página disponível em `localhost:3000`
## Links Úteis : 
 - [Tutoriais NRF24L01](https://forum.arduino.cc/t/simple-nrf24l01-2-4ghz-transceiver-demo/405123/64)
 - [Documentação da biblioteca NRF24](https://nrf24.github.io/RF24/)