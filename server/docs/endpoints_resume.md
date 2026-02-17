# Validação do Token
## Response
| Código | Descrição |
| --- | --- |
| 400 | Token ausente ou malformatado |
| 401 | Token expirado |

# Autenticação

## /auth

<a id="post-auth"></a>
### POST
| Parâmetro | Tipo | Obrigatório | Local | Padrão |
| --- | --- | --- | --- | --- |
| **obrigatórios** |
| cpf | string | Sim | Body | - |
| password | string | Sim | Body | - |

**Response**
| Código | Descrição |
| --- | --- |
| 400 | parâmetros ausentes |
| 401 | usuário ou senha incorretos |

**Response 200**

| Parâmetro | Tipo |
| --- | --- |
| token | string |

## /auth/validateToken

<a id="get-auth-validateToken"></a>
### GET

[Validação do Token](#validação-do-token)

| Parâmetro | Tipo | Obrigatório | Local | Padrão |
| --- | --- | --- | --- | --- |
| **obrigatórios** |
| Authorization | Bearer Token | Sim | Header | - | 

**Response 200**

| Parâmetro | Tipo |
| --- | --- |
| message | string |

# Usuário

## /user

<a id="get-user"></a>
### GET

[Validação do Token](#validação-do-token)

| Parâmetro | Tipo | Obrigatório | Local | Padrão |
| --- | --- | --- | --- | --- |
| **obrigatórios** |
| Authorization | Bearer Token | Sim | Header | - | 

**Response**
| Código | Tipo |
| --- | --- |
| 200 | [user](#user) |

<a id="post-user"></a>
### POST

| Parâmetro | Tipo | Obrigatório | Local | Padrão |
| --- | --- | --- | --- | --- |
| **obrigatórios** |
| cpf | string | Sim | Body | - |
| password | string | Sim | Body | - |
| name | string | Sim | Body | - |

## /user/{ident}

<a id="get-user-ident"></a>
### GET

[Validação do Token](#validação-do-token)

| Parâmetro | Tipo | Obrigatório | Local | Padrão |
| --- | --- | --- | --- | --- |
| **obrigatórios** |
| Authorization | Bearer Token | Sim | Header | - | 
| ident | string | Sim | Path | - |

**Response**
| Código | Tipo |
| --- | --- |
| 200 | [user](#user) |

## /user/updateRules

<a id="post-updateRules"></a>
### POST

[Validação do Token](#validação-do-token)

> Nota: Permissão `UPDATE_USER_RULES` necessária

| Parâmetro | Tipo | Obrigatório | Local | Padrão |
| --- | --- | --- | --- | --- |
| **obrigatórios** |
| Authorization | Bearer Token | Sim | Header | - | 
| user_id | string | Sim | Body | `AUTHENTICATED_USER_ID` |
| **opcionais** |
| to_add | string[] | Não | Body | - |
| to_remove | string[] | Não | Body | - |

**Response**
| Código | Descrição |
| --- | --- |
| 400 | regras inválidas ou argumentos insuficientes |
| 401 | falta de autorização |
| 404 | usuário não encontrado |

# Máquina

## /machine

<a id="post-machine"></a>
### POST

[Validação do Token](#validação-do-token)

> Nota: Permissão `ADD_MACHINE` necessária caso **owner_id** ou **owner_cpf** seja fornecido

| Parâmetro | Tipo | Obrigatório | Local | Padrão |
| --- | --- | --- | --- | --- |
| **obrigatórios** |
| Authorization | Bearer Token | Sim | Header | - | 
| ownerId | string | Sim | Body | `AUTHENTICATED_USER_ID` | 
| networkAdapters | [networkAdapter[]](#networkadapter) | Sim | Body | - |
| os | string | Sim | Body | - |
| title | string | Sim | Body | - |
| **opcionais** |
| ownerCpf | string | Não | Body | - |
| osArchitecture | string | Não | Body | - |
| osInstallDate | datetime | Não | Body | - |
| osVersion | string | Não | Body | - |
| osSerialNumber | string | Não | Body | - |
| organization | string | Não | Body | - |
| motherboard | string | Não | Body | - |
| motherboardManufacturer | string | Não | Body | - |
| processor | string | Não | Body | - |
| processorClockSpeed | integer | Não | Body | - |

**Response**
| Código | Descrição |
| --- | --- |
| 400 | parâmetros inválidos |
| 401 | falta de autorização |
| 404 | usuário não encontrado |
| 200 | [machineWithOwner](#machinewithowner) |

## /machine/{id}

<a id="get-machine-id"></a>
### GET

[Validação do Token](#validação-do-token)

> Nota: Permissão `QUERY_OTHER_USERS` necessária caso a máquina não seja do usuário autenticado

| Parâmetro | Tipo | Obrigatório | Local | Padrão |
| --- | --- | --- | --- | --- |
| **obrigatórios** |
| Authorization | Bearer Token | Sim | Header | - | 
| id | string | Sim | Path  | - | 

**Response**
| Código | Descrição |
| --- | --- |
| 404 | máquina não encontrada |
| 200 | [machineWithOwner](#machinewithowner) |

## /machines
Alias para [/machines/{ident}](#machinesident), onde **ident** corresponde ao usuário autenticado.

## /machines/{ident}

<a id="get-machine-id"></a>
### GET

[Validação do Token](#validação-do-token)

> Nota: Exige a permissão `QUERY_OTHER_USERS` caso **ident** seja de um usuário diferente do autenticado

| Parâmetro | Tipo | Obrigatório | Local | Padrão |
| --- | --- | --- | --- | --- |
| **obrigatórios** |
| Authorization | Bearer Token | Sim | Header | - | 
| ident | string | Sim | Path  | - |

**Response**
| Código | Descrição |
| --- | --- |
| 401 | falta de autorização |
| 404 | usuário não encontrado |

**Response 200**
| Parâmetro | Tipo |
| --- | --- |
| userId | string |
| userName | string |


# Estruturas

## user
| Parâmetro | Tipo | Descrição |
| --- | --- | --- |
| id | string |
| name | string |
| cpf | string |
| rules | string[] |

## userWithMachine
| Parâmetro | Tipo | Descrição |
| --- | --- | --- |
| id | string |
| name | string |
| cpf | string |
| rules | string[] |
| machines | [machine[]](#machine) |

## machine
| Parâmetro | Tipo | Descrição |
| --- | --- | --- |
| id | string |
| mac | string |
| versions | [machineVersion[]](#machineversion) |

## machineWithOwner
| Parâmetro | Tipo | Descrição |
| --- | --- | --- |
| id | string |
| mac | string |
| ownerId | string |
| ownerName | string |
| versions | [machineVersion[]](#machineversion) |

## machineVersion
| Parâmetro | Tipo | Descrição |
| --- | --- | --- |
| id | string |
| configDate | datetime |
| os | string |
| title | string |
| osArchitecture | string |
| osInstallDate | datetime |
| osVersion | string |
| osSerialNumber | string |
| organization | string |
| motherboard | string |
| motherboardManufacturer | string |
| processor | string |
| processorClockSpeed | integer |

## networkAdapter
| Parâmetro | Tipo | Descrição |
| --- | --- | --- |
| name | string |
| mac | string |