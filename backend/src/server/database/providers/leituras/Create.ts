import { ILeitura } from "../../models";
import {Knex} from "../../knex";
import { ETableNames } from "../../ETableNames";

export const create = async(leitura:Omit<ILeitura,'id_leitura'>):Promise<number | Error> => {
    try{
        // Salvando novo registro
        const[result] = await Knex(ETableNames.leitura).insert(leitura).returning('id_leitura');

        // Para lidar com os retornos do SQLITE , que retorna o número ou objeto 
        if (typeof result ==='object') return result.id_leitura;
        if (typeof result ==='number') return result;

        return new Error('Erro ao cadastrar registro de leitura');
  
    } catch(error){
        console.log(error);
        return new Error('Erro ao cadastrar registro de leitura');
  
  }
};