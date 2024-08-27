import * as deleteById from './DeleteById'
import * as getById from './GetById'
import * as create from './Create'
import * as getAll from './GetAll'
import * as count from './Count'

export const LeiturasProvider = {
    ...deleteById,
    ...getById,
    ...create,
    ...getAll,
    ...count,
};

