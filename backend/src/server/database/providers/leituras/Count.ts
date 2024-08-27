import {Knex} from "../../knex";
import { ETableNames } from "../../ETableNames";
import { number } from "yup";

export const count = async(filter =''):Promise<number | Error> => {
    try{
        const [{count}] = await Knex(ETableNames.leitura)
            .where('id_placa','=',`%${filter}%`)
            .count<[{count:number}]>('* as count');

        if (Number.isInteger(Number(count))) return Number(count);

        return new Error('Erro ao consultar a quantidade total de registros de leituras');
  
    } catch(error){
        console.log(error);
        return new Error('Erro ao consultar a quantidade total de registros de leituras');
  
  }
};