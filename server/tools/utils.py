#############################################################
# TOOLS/UTILS
# -----------
# Descrição: Funções utilitárias para o projeto
# Data de Criação: 09/02/2026
# Autor: Iago Carvalho
#############################################################

import os, jwt
import datetime as dt
from flask import jsonify, request, Response
from typing import Callable
from functools import wraps

from models.user import User

def generate_token(userId:str, duration:dt.timedelta=None):
    """ gera um token para o usuário informado com duração de 30 minutos """
    if duration is None:
        duration = dt.timedelta(minutes=30)

    return 'Bearer ' + jwt.encode(
        {"userId": userId, "exp": dt.datetime.now(dt.UTC) + duration},
        os.environ['SECRET_KEY'],
        algorithm="HS256"
    )

def token_required(return_user_id=False, return_user_instance=False) -> Callable[..., tuple[Response, int] | Response]:
    def decotator(func):
        @wraps(func)
        def wrapper(*args, **kwargs):
            secret_key = os.environ['SECRET_KEY']

            # Obtém o token do cabeçalho da requisição
            auth_header = request.headers.get("Authorization")
            if not auth_header:
                return jsonify(message="Token é necessário!"), 400

            parts = auth_header.split()
            if parts[0].lower() != 'bearer' or len(parts) != 2:
                return jsonify(message="Cabeçalho de autorização malformado!"), 400
            token = parts[1]

            try:
                # Decodifica o token
                decoded = jwt.decode(token, secret_key, algorithms=["HS256"])
                userId = decoded['userId']

                if return_user_id:
                    user = userId

                elif return_user_instance:
                    user = User.query.get(userId)

                    if user is None:
                        raise ValueError()
                
            except (jwt.ExpiredSignatureError, ValueError):
                return jsonify(message="Token expirado! Faça login novamente."), 401
            
            except (jwt.InvalidTokenError, KeyError):
                return jsonify(message="Token inválido!"), 400
            
            if return_user_id or return_user_instance:
                return func(user, *args, **kwargs)
            else:
                return func(*args, **kwargs)

        return wrapper
    return decotator

def validate_cpf(cpf:str) -> bool:
    return cpf.isdigit() and len(cpf) == 11

def get_default_mac(values:list[dict[str, str]]):
    for v in values:
        name = v['name'].lower()
        if 'ethernet' in name or 'realtek' in name:
            return v['mac']
    
    # caso não haja nenhuma correspondência, retorna o primeiro
    with_mac = list(filter(lambda v: v['mac'], values))
    return with_mac[0]['mac'] if with_mac else None