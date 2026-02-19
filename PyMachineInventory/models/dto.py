from dataclasses import dataclass

@dataclass
class NewUserDTO:
    cpf: str
    password: str
    name: str

@dataclass
class UserDTO:
    id: str
    cpf: str
    name: str
    rules: list[str]