
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
from sqlalchemy import select

from models.database import db
from models.user import UserModel
from models.machine import *
from tools.utils import get_default_mac, token_required

machine_bp = flask.Blueprint('machine', __name__)

@machine_bp.route('/machine', methods=['POST'])
@token_required(return_user_instance=True)
def post_machine(logged_user:UserModel):
    data = flask.request.get_json()
    owner_by_id = data.get('ownerId')
    owner_by_cpf = data.get('ownerCpf')

    # caso a requisição seja para um usuário diferente do logado, verificar permissão
    if (owner_by_id or owner_by_cpf) and logged_user.cpf != owner_by_cpf and logged_user.id != owner_by_id:
        if not logged_user.check_rule(UserModel.RULE_ADD_MACHINE):
            return flask.jsonify(message='Você não possui permissão para cadastrar a máquina de outro usuário'), 401
        
        if owner_by_id and owner_by_cpf:
            return flask.jsonify(message="forneça apenas um dos parâmetros: 'ownerId' ou 'ownerCpf'"), 400
        
        owner = UserModel.query.get(owner_by_id) if owner_by_id else UserModel.query.filter_by(cpf=owner_by_cpf).first()

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
        
        version = MachineVersionModel.new(
            return_type='dict',
            data=MachineVersionData(
            version=MachineVersionModel(
                mac=mac,
                owner_id=owner.id,
            ),
            machine=MachineModel(
                os=data['os'], title=data['title'], os_architecture=data.get('osArchitecture'),
                os_install_date=os_install_date, os_version=data.get('osVersion'),
                os_serial_number=data.get('osSerialNumber'), organization=data.get('organization'),
                motherboard=data.get('motherboard'), motherboard_manufacturer=data.get('motherboardManufacturer'),
                processor=data.get('processor'), processor_clock_speed=data.get('processorClockSpeed'),
            ),
            disks=[
                DiskModel(name=o['name'], serial_number=o.get('serialNumber'), size=o['size'], model=o.get('model')) for o in data.get('disks', [])
            ],
            adapters=[
                NetworkAdapterModel(name=o['name'], mac=o['mac']) for o in data.get('networkAdapters', [])
            ],
            memories=[
                PhysicalMemoryModel( name=o['name'], capacity=o['capacity'], speed=o['speed']) for o in data.get('physicalMemories', [])
            ],
            programs=[
                ProgramModel(
                    name=o['name'], publisher=o.get('publisher'), version=o.get('version'),
                    estimated_size=o['estimatedSize'], current_user_only=o['currentUserOnly']
                ) for o in data.get('programs', [])
            ]
        ))

        db.session.commit()

    except KeyError:
        return flask.jsonify(message="'networkAdapters', 'title' e 'os' devem ser fornecidos!"), 400
    
    except ValueError: # esperado do tratamento de os_install_date
        return flask.jsonify(message="'osInstallDate' deve seguir o formato 'AAAA-MM-DD HH:MM:SS'"), 400
    
    except Exception as e:
        db.session.rollback()
        log.error(f'new machine error: {e}')
        return flask.jsonify(message="internal server error"), 500
    
    return flask.jsonify(version)

@machine_bp.route('/machine/<mac>')
@token_required(return_user_instance=True)
def get_machine(user:UserModel, mac:str):
    ownerId = flask.request.args.get('ownerId', user.id)
    if 'ownerId' in flask.request.args and user.id != ownerId and not user.check_rule(UserModel.RULE_QUERY_OTHER_USERS):
        return flask.jsonify(message="você não possui autorização para fazer esta requisição"), 401

    version = MachineVersionModel.query\
        .filter_by(mac=mac, owner_id=ownerId)\
        .order_by(MachineVersionModel.datetime.desc())\
        .first()
    
    if not version:
        return flask.jsonify(message="máquina não encontrada!"), 404

    return flask.jsonify(version.dto())

@machine_bp.route('/machines')
@token_required(return_user_id=True)
def get_machines_current_user(user_id:str):
    return flask.redirect(f'/machines/{user_id}')

@machine_bp.route('/machines/<ident>')
@token_required(return_user_instance=True)
def get_machines(user:UserModel, ident:str):

    if user.id != ident and user.cpf != ident: # caso não seja o usuário autenticado
        if not user.check_rule(UserModel.RULE_QUERY_OTHER_USERS):
            return flask.jsonify(message="Você não possui permissão para a consulta de outros usuários"), 401
        
        owner = UserModel.query.filter((UserModel.id == ident) | (UserModel.cpf == ident)).first()
        
        if not owner:
            return flask.jsonify(message="usuário não encontrado!"), 404
        
    else:
        owner = user

    # selecionando a ultima versão de cada mac vinculado ao usuario
    stmt = (
        select(MachineVersionModel.mac)
        .distinct(MachineVersionModel.mac)
        .filter(MachineVersionModel.owner == owner)
    )

    macs_user = db.session.execute(stmt).scalars().all()
    machines = [ MachineVersionModel.get(mac, 'mac', owner.id, as_dict=True, dict_ignore={'machine_version_id', 'owner_id'}) for mac in macs_user ]
    return flask.jsonify(user_id=owner.id, user_name=owner.name, machines=machines)