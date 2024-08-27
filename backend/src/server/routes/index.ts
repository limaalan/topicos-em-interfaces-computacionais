import { Router } from 'express' ;

import {LeiturasController} from './../controllers'
//import { ensureAuthenticated } from '../shared/middlewares';


const router = Router();

router.get('/', (req,res)=>{
    const html = `<h1> API Node com Typescript</h1>
    <h2>Rotas disponíveis  :</h2>
    <ul >
        <li> GET /leituras</li>
        <li> POST /leituras</li>
        <li> GET /leituras:id</li>
        <li> PUT /leituras:id</li>
        <li> DELETE /leituras:id</li>
    </ul>`
    return res.send (html);
});

//Rotas de leituras 
router.get('/leituras', LeiturasController.getAllValidation, LeiturasController.getAll,); // Obtém todas leituras 
router.post('/leituras',LeiturasController.createValidation, LeiturasController.create,); // Cria uma nova pessoa
router.get('/leituras/:id', LeiturasController.getByIdValidation, LeiturasController.getById,); // Obtém uma pessoa de id = id 
router.delete('/leituras/:id', LeiturasController.deleteByIdValidation, LeiturasController.deleteById,); // Deleta uma pessoa de id = id 

export {router };