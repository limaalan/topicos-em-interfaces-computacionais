import { Request, RequestHandler, Response, query } from "express"
import { StatusCodes } from "http-status-codes";
import * as yup from 'yup';
import { validation  } from "../../shared/middlewares";
import { LeiturasProvider } from "../../database/providers/leituras";


interface IParamProps {
    id_leitura ? : number ;
}

const paramsValidation:yup.ObjectSchema<IParamProps> = yup.object().shape({
    id_leitura:   yup.number().integer().required().moreThan(0) ,
});//Cria a validação dos parâmetros

export const getByIdValidation = validation({
    params : paramsValidation,
}); // Passando a validação para uma função que cria o midddleware

export const getById:RequestHandler = async (req:Request<IParamProps>, res:Response) => { 

    if (!req.params.id_leitura) { 
        return res.status(StatusCodes.BAD_REQUEST).json({
            errors:{
                default:"O Parâmetro id_leitura é necessário"
            }
        });
    }; // Nunca vai acontecer isso aqui

    const result = await LeiturasProvider.getById(req.params.id_leitura);
    if (result instanceof Error){
        return res.status(StatusCodes.INTERNAL_SERVER_ERROR).json({
            errors:{
                default:result.message
            }
        });
    };
   
    return res.status(StatusCodes.OK).json(result);

}
