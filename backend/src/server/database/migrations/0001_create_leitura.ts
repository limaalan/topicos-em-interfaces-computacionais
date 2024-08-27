import type { Knex } from "knex";
import { ETableNames } from "../ETableNames";


export async function up(knex: Knex) {
    return knex
    .schema
    .createTable(ETableNames.leitura,table =>{
        table.bigIncrements('id_leitura').primary().index();
        table.integer('id_placa').index().notNullable();
        table.float('temperatura').notNullable();
        table.float('humidade').notNullable();
        table.dateTime('data').notNullable();

        // table
        //     .bigInteger('id_placa')
        //     .index()
        //     .notNullable()
        //     .references('id_placa') // diz que o campo é FK da tablea cidade
        //     .inTable(ETableNames.placa)
        //     .onUpdate('CASCADE') // Ex ID da cidade é alterado, para não perder a referência
        //     .onDelete('RESTRICT'); // Outra opção : SET NULL (sem o notNullable).
        
        table.comment('Tabela de leitura da temepratura') // Comentário 
    }).then(()=>{
        console.log(`#Create table ${ETableNames.leitura}`); // Log no terminal para confirmar que deu certo a migrate
    })
}


export async function down(knex: Knex) {
    return knex.schema.dropTable(ETableNames.leitura).then(()=>{console.log(`# Drop table ${ETableNames.leitura}`)});
}

