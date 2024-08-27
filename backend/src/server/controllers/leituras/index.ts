 import * as deleteById from './DeleteById'
 import * as getById from './GetById';
 import * as create from './Create'
 import * as getAll from './GetAll'

export const LeiturasController = {
    ...deleteById,
    ...getById,
    ...create,
    ...getAll,
};

