# MachineInventoryAPI
API para o inventário de máquinas. Armazena os dados da máquina, criando um versionamento dos dados coletados.

# Exemplos

## Consulta dos usuários de determinado grupo
1. Estando autenticado e membro de um grupo acima de **Acesso ABC - Supervisor**, consulta os membros ativos do grupo de id **abc1**
    ``` powershell
    $ curl "http://localhost:5050/users?groupId=abc1&groupStatus=1" \
        -H "Authorization: Bearer API_KEY"

    [
        {
            "id": "0",
            "name": "Marcelo Araújo",
            "email": "marcelo@email.com",
            "cpf": "000.000.000-00",
            "groups": [
                {
                    "groupId": "abc1",
                    "groupName": "Acesso ABC - Supervisor",
                    "groupDesc": "Supervidores do Acesso ABC",
                    "status" : TRUE,
                    "rules": [
                        "ruleId": "RULE_CRUD_USER",
                        "ruleDesc": "Leitura, criação, remoção e atualização dos usuários e grupos abaixo deste"
                    ]
                },
                {
                    "groupId": "abc2",
                    "groupName": "Acesso ABC - Visitante",
                    "groupDesc": "Visitantes do Acesso ABC",
                    "status" : True,
                    "rules": []
                }
            ]
        }
    ]
    ```