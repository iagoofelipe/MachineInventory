# Gerencimaneto de Usuário
Endpoints para o gerenciamento de usuários. 

| Endpoint | Método | Descripção |
| --- | --- | --- |
| [/users](#get-users) | `GET` | Retorna uma lista com todos os usuários |
| [/user](#post-user) | `POST` | Cria um novo usuário |
| [/user/{id}](#get-user) | `GET` | Retorna os dados do usuário |
| [/user/{id}](#put-user) | `PUT` | Atualiza os dados do usuário |
| [/user/{id}](#delete-user) | `DELETE` | Apaga os dados do usuário |

# /users
> Este endpoint possui validação de token, para mais detalhes, consule [Autenticação | Validação do Token](../authentication.md#validação-do-token).

<a id="get-users"></a>
## GET

Retorna uma lista com todos os usuários.

>**Nota:** A consulta é restrita a usuários do mesmo grupo ou subgrupo. Os grupos disponíveis são restritos ao escopo de grupos do usuário autenticado e que possuem a regra `RULE_READ_USER`.

| Parâmetro | Tipo | Obrigatório | Opções | Padrão | Local | Descrição |
| --- | --- | --- | --- | --- | --- | --- |
| `Token JWT` | string | Sim | - | - | Header | Token de autenticação gerado em [/auth](auth.md#auth) |
| `groupId` | string| Não | - | - | Query | Filtragem dos usuários que estão no grupo desejado |
| `groupStatus` | int | Não | 0 \| 1 | - | Query | Fitra os usuários que estão ativos ou inativos no grupo (caso `groupId` não seja fornecido, será aplicado a todos os grupos) |

### Response

É retornado um objeto [message](../structs.md#message) contendo o resultado da operação.

| Code | Descrição |
| --- | --- |
| `400` | erro na validação dos parâmetros |
| `403` | falta de autorização |

### Response 200
Em caso de êxito, é retornado um array contendo os seguintes campos:
- `id` **string** : ID do usuário
- `name` **string** : Nome completo do usuário
- `email` **string** : Endereço de e-mail do usuário
- `cpf` **string** : CPF do usuário
- `groups` **array[]** : Lista dos grupos que o usuário faz parte
    - `groupId` **string** : ID do grupo
    - `groupName` **string** : Nome do grupo
    - `groupDesc` **string** : Descrição do grupo
    - `status` **boolean** : Se o usuário está ativo no respectivo grupo
    - `rules` **array[]** : Lista de regras do grupo
        - `ruleId` **string** : ID da regra
        - `ruleDesc` **string** : Descrição da regra

# /user
> Este endpoint possui validação de token, para mais detalhes, consule [Autenticação | Validação do Token](../authentication.md#validação-do-token).

<a id="post-user"></a>
## POST

Criação de um novo usuário.

Para a definição dos grupos, é necessário um usuário autenticado e com permissão de adição de novos membros nos grupos desejados.

| Parâmetro | Tipo | Obrigatório | Opções | Padrão | Local | Descrição |
| --- | --- | --- | --- | --- | --- | --- |
| `name` |  string | Sim | - | - | Body | Nome do usuário |
| `email` |  string | Sim | - | - | Body | E-mail do usuário |
| `cpf` |  string | Sim | - | - | Body | CPF do usuário |
| `password` |  string | Sim | - | - | Body | Senha do usuário |
| `Token JWT` | string | Não | - | - | Header | Token de autenticação gerado em [/auth](auth.md#auth) |
| `groups` | array[] | Não | - | - | Body | ID dos grupos que o usuário fará parte |


### Response

É retornado um objeto [message](../structs.md#message) contendo o resultado da operação.

| Code | Descrição |
| --- | --- |
| `401` | usuário sem permissão |
| `200` | operação com êxito |


# /user/{id}
> Este método possui validação de token, para mais detalhes, consule [Autenticação | Validação do Token](../authentication.md#validação-do-token).

<a id="get-user"></a>
## GET
Retorna os dados de um usuário específico.

>**Nota:** Para a consulta, o usuário autenticado deve fazer parte de um grupo com permissão de leitura e que o usuário desejado esteja abaixo.

| Parâmetro | Tipo | Obrigatório | Opções | Padrão | Local | Descrição |
| --- | --- | --- | --- | --- | --- | --- |
| `Token JWT` | string | Sim | - | - | Header | Token de autenticação gerado em [/auth](auth.md#auth) |
| `id` | string | Sim | - | - | Path | ID do usuário desejado |

### Response
É retornado um objeto [message](../structs.md#message) contendo o resultado da operação.

| Code | Descrição |
| --- | --- |
| `401` | usuário sem permissão |

### Response 200
É retornado um objeto contendo os seguintes campos:

- `id` **string** : ID do usuário
- `name` **string** : Nome completo do usuário
- `email` **string** : Endereço de e-mail do usuário
- `cpf` **string** : CPF do usuário
- `groups` **array[]** : Lista dos grupos que o usuário faz parte
    - `groupId` **string** : ID do grupo
    - `groupName` **string** : Nome do grupo
    - `groupDesc` **string** : Descrição do grupo
    - `status` **boolean** : Se o usuário está ativo no respectivo grupo
    - `rules` **array[]** : Lista de regras do grupo
        - `ruleId` **string** : ID da regra
        - `ruleDesc` **string** : Descrição da regra

<a id="put-user"></a>
## PUT

Atualiza os dados do usuário.

| Parâmetro | Tipo | Obrigatório | Opções | Padrão | Local | Descrição |
| --- | --- | --- | --- | --- | --- | --- |
| `Token JWT` | string | Sim | - | - | Header | Token de autenticação gerado em [/auth](auth.md#auth) |
| `id` | string | Sim | - | - | Path | ID do usuário desejado |
| `name` | string | Não | - | - | Body | Novo nome do usuário |
| `email` | string | Não | - | - | Body | Novo e-mail do usuário |
| `cpf` | string | Não | - | - | Body | Novo CPF do usuário |
| `password` | string | Não | - | - | Body | Nova senha do usuário |

### Response
É retornado um objeto [message](../structs.md#message) contendo o resultado da operação.

| Code | Descrição |
| --- | --- |
| `401` | usuário sem permissão |
| `200` | êxito da operação |

<a id="delete-user"></a>
## DELETE

Exclusão de um usuário.

> **Nota:** Apenas o próprio usuário ou usuários com regra `RULE_DELETE_USER` podem excluir um usuário.

| Parâmetro | Tipo | Obrigatório | Opções | Padrão | Local | Descrição |
| --- | --- | --- | --- | --- | --- | --- |
| `Token JWT` | string | Sim | - | - | Header | Token de autenticação gerado em [/auth](auth.md#auth) |
| `id` | string | Sim | - | - | Path | ID do usuário desejado |

### Response
É retornado um objeto [message](../structs.md#message) contendo o resultado da operação.

| Code | Descrição |
| --- | --- |
| `401` | usuário sem permissão |
| `200` | êxito da operação |
