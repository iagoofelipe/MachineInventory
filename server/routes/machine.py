
#############################################################
# ROUTES/SYSTEM
# -------------
# Descrição: Rotas da API para manipulação de dados da máquina
# Data de Criação: 09/02/2026
# Autor: Iago Carvalho
#############################################################

import flask
import logging as log
import datetime as dt

from models.database import db
from models.user import User
from models.machine import Machine, MachineExtra
from tools.utils import get_default_mac, token_required

machine_bp = flask.Blueprint('machine', __name__)

@machine_bp.route('/machine', methods=['POST'])
@token_required(return_user_instance=True)
def post_machine(logged_user:User):
    data = flask.request.get_json()
    owner_by_id = 'ownerId' in data
    owner_by_cpf = 'ownerCpf' in data

     # caso a requisição seja para um usuário diferente do logado, verificar permissão
    if owner_by_id or owner_by_cpf:
        if not logged_user.check_rule(User.RULE_ADD_MACHINE):
            return flask.jsonify(message='Você não possui permissão para cadastrar a máquina de outro usuário'), 401
        
        if owner_by_id and owner_by_cpf:
            return flask.jsonify(message="forneça apenas um dos parâmetros: 'ownerId' ou 'ownerCpf'"), 400
        
        owner = User.query.get(data['ownerId']) if owner_by_id else User.query.filter_by(cpf=data['ownerCpf']).first()

        if not owner:
            return flask.jsonify(message='Usuário não encontrado'), 404

    else:
        owner = logged_user
        
    try:
        os_install_date = data.get('osInstallDate')
        if os_install_date:
            os_install_date = dt.datetime.strptime(os_install_date, '%Y-%m-%d %H:%M:%S')
            

        # verifica se há uma máquina com esse mac
        mac = get_default_mac(data['networkAdapters'])
        if not mac:
            return flask.jsonify(message="a máquina deve conter pelo menos um endereço mac (networkAdapter)"), 400

        machine = Machine.query.filter_by(mac=mac, owner_id=owner.id).first()

        if not machine:
            machine = Machine(
                mac=mac,
                owner=owner,
            )

            db.session.add(machine)
            db.session.commit()

        new_version = MachineExtra(
            os=data['os'],
            title=data['title'],
            os_architecture=data.get('osArchitecture'),
            os_install_date=os_install_date,
            os_version=data.get('osVersion'),
            os_serial_number=data.get('osSerialNumber'),
            organization=data.get('organization'),
            motherboard=data.get('motherboard'),
            motherboard_manufacturer=data.get('motherboardManufacturer'),
            processor=data.get('processor'),
            processor_clock_speed=data.get('processorClockSpeed'),
            machine=machine,
        )

        db.session.add(new_version)
        db.session.commit()
    
    except KeyError:
        return flask.jsonify(message="'networkAdapters', 'title' e 'os' devem ser fornecidos!"), 400
    
    except ValueError: # esperado do tratamento de os_install_date
        return flask.jsonify(message="'osInstallDate' deve seguir o formato 'AAAA-MM-DD HH:MM:SS'"), 400
    
    except Exception as e:
        db.session.rollback()
        log.error(f'new machine error: {e}')
        return flask.jsonify(message="internal server error"), 500

    return flask.jsonify(machine.dto(versions=True, owner=True))

@machine_bp.route('/machine/<id>')
@token_required(return_user_instance=True)
def get_machine(user:User, id:str):
    machine = Machine.query.get(id)
    if not machine:
        return flask.jsonify(message="máquina não encontrada!"), 404

    if user.id != machine.owner.id and not user.check_rule(User.RULE_QUERY_OTHER_USERS):
        return flask.jsonify(message="você não possui autorização para fazer esta requisição"), 401

    return flask.jsonify(machine.dto(versions=True, owner=True))

@machine_bp.route('/machines')
@token_required(return_user_id=True)
def get_machines_current_user(user_id:str):
    return flask.redirect(f'/machines/{user_id}')

@machine_bp.route('/machines/<ident>')
@token_required(return_user_instance=True)
def get_machines(user:User, ident:str):

    if user.id != ident and user.cpf != ident: # caso não seja o usuário autenticado
        if not user.check_rule(User.RULE_QUERY_OTHER_USERS):
            return flask.jsonify(message="Você não possui permissão para a consulta de outros usuários"), 401
        
        owner = User.query.filter((User.id == ident) | (User.cpf == ident)).first()
        
        if not owner:
            return flask.jsonify(message="usuário não encontrado!"), 404
        
    else:
        owner = user


    return flask.jsonify(userId=owner.id, userName=owner.name, machines=[m.dto(versions=True) for m in owner.machines])