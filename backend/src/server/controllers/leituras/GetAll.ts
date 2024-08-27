import { Request, RequestHandler, Response, query } from "express"
import { StatusCodes } from "http-status-codes";
import * as yup from 'yup';
import { validation  } from "../../shared/middlewares";
import { LeiturasProvider } from "../../database/providers/leituras";


interface IQueryProps {
    page? : number ;
    limit? : number ;
    filter? : string;
}

const queryValidation:yup.ObjectSchema<IQueryProps> = yup.object().shape({
    page:   yup.number().optional().moreThan(0) ,
    limit:  yup.number().optional().moreThan(0) ,
    filter: yup.string().optional() ,
});//Cria a validação da query

export const getAllValidation = validation({
    query : queryValidation,
}); // Passando a validação para uma função que cria o midddleware

export const getAll:RequestHandler = async (req:Request<{},{},{},IQueryProps>, res:Response) => { 
    
    const result = await LeiturasProvider.getAll(req.query.page || 1, req.query.limit || 7 , req.query.filter || '' );
    const count = await LeiturasProvider.count(req.query.filter);

    console.log(result,count);

    if (result instanceof Error) {
        return res.status(StatusCodes.INTERNAL_SERVER_ERROR).json({
            errors:{default:result.message}
        });
    } else if (count instanceof Error){
        return res.status(StatusCodes.INTERNAL_SERVER_ERROR).json({
            errors:{default:count.message}
        });
    }

    res.setHeader('access-control-expose-headers','x-total-count');
    res.setHeader('x-total-count',count);


    return res.status(StatusCodes.OK).json(result);

};