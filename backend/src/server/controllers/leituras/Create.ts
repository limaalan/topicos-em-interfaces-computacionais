import { Request, RequestHandler, Response, json, query } from "express"
import { StatusCodes } from "http-status-codes";
import * as yup from 'yup';
import { validation  } from "../../shared/middlewares";
import { ILeitura } from "../../database/models";
import { LeiturasProvider } from "../../database/providers/leituras";


interface IBodyProps extends Omit<ILeitura,'id_leitura'> {}

const bodyValidation:yup.ObjectSchema<IBodyProps> = yup.object().shape({
    id_placa : yup.number().required().integer(),
    temperatura: yup.number().required(),
    humidade: yup.number().required().lessThan(100) ,
    data: yup.date().required(),
});//Cria a validação do body

export const createValidation = validation({
    body : bodyValidation,
}); // Passando a validação para uma função que cria o midddleware

export const create:RequestHandler = async (req:Request<{},{},IBodyProps>, res) => { 
   
    const result = await LeiturasProvider.create(req.body); // Tenta criar leitura
    if( result instanceof Error) { // Caso erro, retorna mensagem de erro da provider
        return res.status(StatusCodes.INTERNAL_SERVER_ERROR).json({
            errors: {
                default: result.message
            }
            
        })
    }

    return res.status(StatusCodes.CREATED).json(result); // Caso deu certo retorna o id da leitura

}