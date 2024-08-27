import {compare, genSalt, hash} from 'bcryptjs';
const SALT_RANDOMS = 8; // Tamanho do salt

const hashPassword = async (password:string)=>{
    const saltGenerated = await genSalt(SALT_RANDOMS); // Gera um salt com o tamanho declarado
    return await hash(password,saltGenerated);

};

const verifyPassword = async (password:string, hashedPassword:string)=>{
    return await compare(password,hashedPassword); // Verifica se a senha informada é igual a armazenada
};

export const PasswordCrypto = {
    hashPassword,
    verifyPassword,
}