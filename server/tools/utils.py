#############################################################
# TOOLS/UTILS
# -----------
# Descrição: Funções utilitárias para o projeto
# Data de Criação: 09/02/2026
# Autor: Iago Carvalho
#############################################################

import os, secrets, jwt
import logging as log
from configparser import ConfigParser
from flask import jsonify, request
from functools import wraps

from tools.consts import FILE_CONFIG

def get_secret_key():
    if 'SECRET_KEY' in os.environ:
        return os.environ['SECRET_KEY']

    config = ConfigParser()
    
    if os.path.exists(FILE_CONFIG):
        config.read(FILE_CONFIG)

    if config.has_option('flask', 'SECRET_KEY'):
        key = os.environ['SECRET_KEY'] = config.get('flask', 'SECRET_KEY')
    
    else:
        log.warning(f'flask SECRET_KEY not set, verify the "{FILE_CONFIG}" file')
        key = os.environ['SECRET_KEY'] = secrets.token_urlsafe(32)

    return key

def token_required(secret_key:str=None):
    def decorator(func):
        
        @wraps(func)
        def wrapper(*args, **kwargs):
            nonlocal secret_key
            
            if secret_key is None:
                secret_key = get_secret_key()

            # Obtém o token do cabeçalho da requisição
            auth_header = request.headers.get("Authorization")
            if not auth_header:
                return jsonify(message="Token é necessário!"), 403

            parts = auth_header.split()
            if parts[0].lower() != 'bearer' or len(parts) != 2:
                return jsonify(message="Cabeçalho de autorização malformado!"), 401
            token = parts[1]

            try:
                # Decodifica o token
                decoded = jwt.decode(token, secret_key, algorithms=["HS256"])
                user = decoded['user']
            except jwt.ExpiredSignatureError:
                return jsonify(message="Token expirado! Faça login novamente."), 401
            
            except jwt.InvalidTokenError:
                return jsonify(message="Token inválido!"), 403
            
            return func(user, *args, **kwargs)
        return wrapper
    return decorator