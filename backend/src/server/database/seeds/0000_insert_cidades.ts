import { Knex } from "knex";
import { ETableNames } from "../ETableNames";
import { ILeitura } from "../models";

export const seed = async (knex: Knex) => {
    const [{ count }] = await knex(ETableNames.leitura).count<[{ count: number }]>("* as count"  );
    if (!Number.isInteger(count) || Number(count) > 0) return;

    await knex(ETableNames.leitura).insert(leiturasFake);
};

const leiturasFake:ILeitura[] = [{
  id_leitura:1,
  id_placa:1,
  temperatura:20,
  humidade:20,
  data: new Date(),
},
{
  id_leitura:2,
  id_placa:99,
  temperatura:20,
  humidade:20,
  data: new Date(),
},
{
  id_leitura:3,
  id_placa:1,
  temperatura:20,
  humidade:20,
  data: new Date(),
},
{
  id_leitura:4,
  id_placa:99,
  temperatura:20,
  humidade:20,
  data: new Date(),
},]