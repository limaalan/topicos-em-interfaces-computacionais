import { ILeitura} from "../../models"
declare module 'knex/types/tables'{
    interface Tables{
        leitura: ILeitura  
    }
}