
#############################################################
# ROUTES/SYSTEM
# -------------
# Descrição: Rotas da API para manipulação de dados da máquina
# Data de Criação: 09/02/2026
# Autor: Iago Carvalho
#############################################################

import flask

system_bp = flask.Blueprint('system', __name__)

@system_bp.route('/system/newMachine', methods=['POST'])
def post_new_machine():
    return { 'success': True, 'message': 'Máquina cadastrada com sucesso!' }