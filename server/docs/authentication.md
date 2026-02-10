# Autenticação
Validações e métodos necessários para a autenticação na API.

Esta API utiliza a autenticação `JWT`, o qual gera um token de acesso para que os serviços sejam acessados conforme as regras definidas em cada grupo de usuário. Impedindo acessos ou operações indevidas.

# Validação do Token

O token é gerado através do endpoint [/auth](endpoints/auth.md#auth) e possui validade de 30 minutos. Todos os endpoints que precisam de determinado nível de acesso solicitarão esse token. Após a geração, basta fornecer o valor retornado do endpoint no cabeçalho das requisições, com a chave `Authentication`. Após o prazo de 30 minutos, um novo token é requerido, sendo necessária uma nova autenticação.

## Response

A validação segue alguns processos que podem retornar respostas diferentes dependendo da requisição. Abaixo foram listadas as validações prévias que todo endpoint, o qual utiliza o `Token JWT`, seguem.

| Response Code | Descrição |
| --- | --- |
| `400` | Token ausente, inválido ou malformatado |
| `401` | Token expirado |

Além do código de retorno, é retornada um objeto [message](./structs.md#message) contendo informações do erro.


> **Nota:** Caso a validação seja finalizada com êxito, o processo de requisição seguirá as próximas etapas do endpoint correspondente.