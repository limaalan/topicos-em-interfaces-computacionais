import { RequestHandler } from "express";
import { StatusCodes } from "http-status-codes";
import { ObjectSchema,ValidationError } from "yup";


type TProperty = 'body'|'header'|'params'|'query'
type TAllSchemas = Record<TProperty,ObjectSchema<any>>;

type TValidation = (schemas:Partial<TAllSchemas>) => RequestHandler ; // Recebe tipo de campo e esquema a validar , Partial deixa que os campos não sejam obrigatórios

export const validation:TValidation = (schemas) => async (req,res,next) => {  
    const errorsResult : Record<string, Record<string, string>> = {};

    Object.entries(schemas).forEach(([key,schema])=> {    
        try {
                schema.validateSync(req[key as TProperty], {abortEarly:false});
        } catch (err) {
                const yupError = err as ValidationError;
                const errors : Record<string, string> = {}

                yupError.inner.forEach(error => {
                    if (error.path === undefined) return;
                    errors[error.path] = error.message ;
                });

                errorsResult[key] = errors;
        }
    });

    //Checa se houve algum erro
    if(Object.entries(errorsResult).length === 0) {
        return next(); // Caso não, segue para a criação 
    } else {
        return res.status(StatusCodes.BAD_REQUEST).json({ errors:errorsResult }); // Caso sim, retrona objeto dos erros 
    }

};