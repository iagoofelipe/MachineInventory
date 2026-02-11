
#############################################################
# MODELS/DTO
# ----------
# Descrição: Data Transfer Objects, estruturas para transferência de objetos
# Data de Criação: 09/02/2026
# Autor: Iago Carvalho
#############################################################

from dataclasses import dataclass

@dataclass
class UserDTO:
    id: str
    name: str
    cpf: str