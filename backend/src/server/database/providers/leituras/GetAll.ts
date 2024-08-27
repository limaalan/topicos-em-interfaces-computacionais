import {ILeitura } from "../../models";
import {Knex} from "../../knex";
import { ETableNames } from "../../ETableNames";

export const getAll = async(page:number, limit:number, filter:string ):Promise<ILeitura[] | Error> => {
    try{
        const result = await Knex(ETableNames.leitura)
            .select('*')
            //.where('id_placa','=',`%${filter}%`) // % para qualquer coisa antes ou depois.
            .offset((page-1)*limit)
            .limit(limit);
            // AQUI traz todas
        return result;

    } catch(error){
        console.log(error);
        return new Error('Erro ao consultar os registros de leitura');
  
  }
};