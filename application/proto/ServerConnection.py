import os
import requests

class ServerConnection:
    BASE_URL = 'http://127.0.0.1:5000'

    def __init__(self):
        self.last_error = ''
        self.token = ''

        if os.path.exists('token.txt'):
            with open('token.txt') as f:
                self.token = f.readline()

            if not self.validateToken():
                self.token = ''

    def getLastError(self) -> str:
        return self.last_error
    
    def hasToken(self):
        return bool(self.token)
    
    def validateToken(self):
        response = requests.get(self.BASE_URL + '/auth/validateToken', headers={ 'Authorization': self.token })
        return response.status_code == 200

    def generateToken(self, cpf:str, password:str) -> bool:
        response = requests.post(self.BASE_URL + '/auth', json={ 'cpf': cpf, 'password': password })
        data = response.json()

        if response.status_code != 200:
            self.last_error = data['message']
            return False
        
        with open('token.txt', 'w') as f:
            self.token = data['token']
            f.write(self.token)

        return True

    def getUser(self, cpf_or_id:str=None) -> dict | None:
        url = self.BASE_URL + '/user'
        if cpf_or_id:
            url += '/' + cpf_or_id
        response = requests.get(url, headers={ 'Authorization': self.token })
        data = response.json()

        if response.status_code != 200:
            self.last_error = data['message']
            return
        
        return data