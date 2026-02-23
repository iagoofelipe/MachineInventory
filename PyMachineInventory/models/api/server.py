import os
import requests
from requests.exceptions import ConnectionError

from .. import dto
from tools.utils import dataclass_to_dict

TOKEN_FILE = os.path.join(os.environ.get('TMP', '.'), 'PyMachineInventory.ini')

class ServerAPI:
    BASE_URL = 'http://127.0.0.1:5000'

    def __init__(self):
        self._last_error = ''
        self._headers = { 'Authorization': '' }

    def getLastError(self) -> str:
        return self._last_error
    
    def logout(self):
        if os.path.exists(TOKEN_FILE):
            os.remove(TOKEN_FILE)

        self._headers['Authorization'] = ''

    def readToken(self):
        if os.path.exists(TOKEN_FILE):
            with open(TOKEN_FILE) as f:
                self._headers['Authorization'] = f.readline()
        
        return self.hasToken()
        
    def hasToken(self):
        return bool(self._headers['Authorization'])
    
    def validateToken(self):
        try:
            response = requests.get(self.BASE_URL + '/auth/validateToken', headers=self._headers)
        except ConnectionError:
            self._last_error = 'erro de conexão com o servidor'
            return False
        
        return response.status_code == 200

    def generateToken(self, cpf:str, password:str) -> bool:
        try:
            response = requests.post(self.BASE_URL + '/auth', json={ 'cpf': cpf, 'password': password })
        except ConnectionError:
            self._last_error = 'erro de conexão com o servidor'
            return False
        
        data = response.json()

        if response.status_code != 200:
            self._last_error = data['message']
            return False
        
        with open(TOKEN_FILE, 'w') as f:
            self._headers['Authorization'] = data['token']
            f.write(self._headers['Authorization'])

        return True

    def getUser(self, cpf_or_id:str=None) -> dto.UserDTO | None:
        url = self.BASE_URL + '/user'
        if cpf_or_id:
            url += '/' + cpf_or_id
        
        try:
            response = requests.get(url, headers=self._headers)
        except ConnectionError:
            self._last_error = 'erro de conexão com o servidor'
            return
        
        data = response.json()

        if response.status_code != 200:
            self._last_error = data['message']
            return
        
        return dto.UserDTO(**data)
    
    def newUser(self, user:dto.NewUserDTO) -> bool:
        url = self.BASE_URL + '/user'

        try:
            response = requests.post(url, json=dataclass_to_dict(user), headers=self._headers)
        except ConnectionError:
            self._last_error = 'erro de conexão com o servidor'
            return False
        
        data = response.json()

        if response.status_code != 200:
            self._last_error = data['message']

        return response.status_code == 200
