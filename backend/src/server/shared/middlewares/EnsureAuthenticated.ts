import { RequestHandler } from "express";
import { StatusCodes } from "http-status-codes";
import { JWTService } from "../services";

export const ensureAuthenticated:RequestHandler =async (req, res, next) =>{
    const {authorization} = req.headers;
    // Checa se tem o header auth
    if (!authorization){
        return res.status(StatusCodes.UNAUTHORIZED).json({
            errors:{default:"Não autenticado!"}
        });

    };
    // Checa se o tipo do token é 'Bearer'
    const[type, token] = authorization.split(' ');
    if (type !== 'Bearer'){
        return res.status(StatusCodes.UNAUTHORIZED).json({
            errors:{default:"Não autenticado!"}
        });
    }
    // Checa se o token é válido
    const JWTData = JWTService.verify(token)
    if(JWTData==="JWT_SECRET_NOT_FOUND"){
        return res.status(StatusCodes.INTERNAL_SERVER_ERROR).json({
            errors:{default:"Erro ao gerar secret"}
        })
    }

    if(JWTData==="INVALID_TOKEN"){
        return res.status(StatusCodes.UNAUTHORIZED).json({
            errors:{default:"Não autenticado!"}
        })
    }
    req.headers.idUsuario = JWTData.uid.toString();
    return next();

};