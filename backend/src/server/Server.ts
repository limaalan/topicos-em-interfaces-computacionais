import express from 'express' ;
import 'dotenv/config';
import './shared/services/TranslationsYup';
import cors from 'cors';
import {router} from './routes';

const server = express();

//server.use(cors({
//    // Lista de todos os endereços que é permitido fazer chamada para esta api
//    origin:process.env.ENABLED_CORS?.split(';')||[]
//}));
server.use(express.json());

server.use(router);

export {server};