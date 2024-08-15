const express = require('express');
const http = require('http');
const { Server } = require('socket.io');
const { SerialPort } = require('serialport');
const { ReadlineParser } = require('@serialport/parser-readline');

const app = express();
const server = http.createServer(app);
const io = new Server(server);

const PORTA_SERIAL = '/dev/ttyUSB0'; // Porta serial do Arduino
const BAUDRATE = 115200; // Taxa de transmissão

// Inicializa a conexão com o Arduino
const arduino = new SerialPort({ path: PORTA_SERIAL, baudRate: BAUDRATE });
const parser = arduino.pipe(new ReadlineParser({ delimiter: '\n' }));

parser.on('data', (data) => {
  const dados = data.toString();
  const parts = dados.split(' ');
  //console.log(dados);
  const id= parts[0];
  const temp=parts[1];
  const humidade=parts[2];
  console.log(`id: ${id}, temp: ${temp}, hum: ${humidade}`);

  io.emit('atualizar-dados',{ id, temp, humidade });
});

app.use(express.static('public'));

server.listen(3000, () => {
  console.log('Servidor rodando em http://localhost:3000');
});
