#############################################################
# TOOLS/UTILS
# -----------
# Descrição: Funções utilitárias para o projeto
# Data de Criação: 09/02/2026
# Autor: Iago Carvalho
#############################################################

import os, jwt, re
from flask import jsonify, request
from functools import wraps

def token_required(secret_key:str=None):
    def decorator(func):
        
        @wraps(func)
        def wrapper(*args, **kwargs):
            nonlocal secret_key
            
            if secret_key is None:
                secret_key = os.environ['SECRET_KEY']

            # Obtém o token do cabeçalho da requisição
            auth_header = request.headers.get("Authorization")
            if not auth_header:
                return jsonify(success=False, msg="Token é necessário!"), 400

            parts = auth_header.split()
            if parts[0].lower() != 'bearer' or len(parts) != 2:
                return jsonify(success=False, msg="Cabeçalho de autorização malformado!"), 400
            token = parts[1]

            try:
                # Decodifica o token
                decoded = jwt.decode(token, secret_key, algorithms=["HS256"])
                user = decoded['user']
            except jwt.ExpiredSignatureError:
                return jsonify(success=False, msg="Token expirado! Faça login novamente."), 401
            
            except jwt.InvalidTokenError:
                return jsonify(success=False, msg="Token inválido!"), 400
            
            return func(user, *args, **kwargs)
        return wrapper
    return decorator

def validate_cpf(cpf:str) -> bool:
    return len(cpf) == 14 and re.match(r'\d{3}.\d{3}.\d{3}-\d{2}', cpf)