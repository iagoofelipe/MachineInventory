import os
import requests

from . import dto
from tools.utils import dataclass_to_dict


class ServerConnection:
    BASE_URL = 'http://127.0.0.1:5000'

    def __init__(self):
        self._last_error = ''
        self._headers = { 'Authorization': '' }

        if os.path.exists('token.txt'):
            with open('token.txt') as f:
                self._headers['Authorization'] = f.readline()

            if not self.validateToken():
                self._headers['Authorization'] = ''

    def getLastError(self) -> str:
        return self._last_error

    def readToken(self):
        if os.path.exists('token.txt'):
            with open('token.txt') as f:
                self._headers['Authorization'] = f.readline()
        
        return self.hasToken()
        
    def hasToken(self):
        return bool(self._headers['Authorization'])
    
    def validateToken(self):
        response = requests.get(self.BASE_URL + '/auth/validateToken', headers=self._headers)
        return response.status_code == 200

    def generateToken(self, cpf:str, password:str) -> bool:
        response = requests.post(self.BASE_URL + '/auth', json={ 'cpf': cpf, 'password': password })
        data = response.json()

        if response.status_code != 200:
            self._last_error = data['message']
            return False
        
        with open('token.txt', 'w') as f:
            self._headers['Authorization'] = data['token']
            f.write(self._headers['Authorization'])

        return True

    def getUser(self, cpf_or_id:str=None) -> dto.UserDTO | None:
        url = self.BASE_URL + '/user'
        if cpf_or_id:
            url += '/' + cpf_or_id
        response = requests.get(url, headers=self._headers)
        data = response.json()

        if response.status_code != 200:
            self._last_error = data['message']
            return
        
        return dto.UserDTO(**data)
    
    def newUser(self, user:dto.NewUserDTO) -> bool:
        url = self.BASE_URL + '/user'
        response = requests.post(url, json=dataclass_to_dict(user), headers=self._headers)
        data = response.json()

        if response.status_code != 200:
            self._last_error = data['message']

        return response.status_code == 200
