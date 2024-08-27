import { Request, RequestHandler, Response, query } from "express"
import { StatusCodes } from "http-status-codes";
import * as yup from 'yup';
import { validation  } from "../../shared/middlewares";
import {  LeiturasProvider} from "../../database/providers/leituras";

interface IParamsProps {
    id_leitura? : number ;
}

const paramsValidation:yup.ObjectSchema<IParamsProps> = yup.object().shape({
    id_leitura: yup.number().required().integer(),
});//Cria a validação dos parâmetros


export const deleteByIdValidation = validation({
    params : paramsValidation,
}); // Passando a validação para uma função que cria o midddleware

export const deleteById:RequestHandler = async (req:Request<IParamsProps>, res:Response) => { 

    if (!req.params.id_leitura) { 
        return res.status(StatusCodes.BAD_REQUEST).json({
            errors:{
                default:"O Parâmetro Id_leitura é necessário"
            }
        });
    }; // Nunca vai acontecer isso aqui

    const result = await LeiturasProvider.deleteById(req.params.id_leitura);
    if (result instanceof Error) {
        return res.status(StatusCodes.INTERNAL_SERVER_ERROR).json({
            errors:{
                default:result.message
            }
        })
    }
    return res.status(StatusCodes.NO_CONTENT).send();

};
