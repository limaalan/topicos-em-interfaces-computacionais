import { ILeitura } from "../../models";
import {Knex} from "../../knex";
import { ETableNames } from "../../ETableNames";

export const getById = async(id:number):Promise<ILeitura | Error> => {
    try{
        const result = await Knex(ETableNames.leitura)
            .select('id_leitura','id_placa','temperatura','humidade','data')
            .where('id_leitura','=',id)
            .first();

        if ( result ) return result;
        return new Error('Registro de leitura não encontrado');
  
    } catch(error){
        console.log(error);
        return new Error('Erro ao consultar registro de leitura ');
  
  }
};