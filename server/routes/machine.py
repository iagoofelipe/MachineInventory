
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

from models.database import User, Machine, MachineExtra, db
from tools.utils import get_default_mac

machine_bp = flask.Blueprint('machine', __name__)

@machine_bp.route('/machine/newMachine', methods=['POST'])
def post_new_machine():
    data = flask.request.get_json()

    try:
        owner = User.query.get(data['owner_id'])
        if not owner:
            return flask.jsonify(success=False, message='Usuário não encontrado'), 404

        os_install_date = data.get('os_install_date')
        if os_install_date:
            os_install_date = dt.datetime.strptime(os_install_date, '%Y-%m-%d %H:%M:%S')
            

        # verifica se há uma máquina com esse mac
        mac = get_default_mac(data['network_adapters'])
        if not mac:
            return flask.jsonify(success=False, message="a máquina deve conter pelo menos um endereço mac (network_adapter)"), 401

        machine = Machine.query.filter_by(mac=mac).first()

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
            os_architecture=data.get('os_architecture'),
            os_install_date=os_install_date,
            os_version=data.get('os_version'),
            os_serial_number=data.get('os_serial_number'),
            organization=data.get('organization'),
            motherboard=data.get('motherboard'),
            motherboard_manufacturer=data.get('motherboard_manufacturer'),
            processor=data.get('processor'),
            processor_clock_speed=data.get('processor_clock_speed'),
            machine=machine,
        )

        db.session.add(new_version)
        db.session.commit()
    
    except KeyError:
        return flask.jsonify(success=False, message="'owner_id', 'network_adapters', 'title' e 'os' devem ser fornecidos!"), 400
    
    except ValueError: # esperado do tratamento de os_install_date
        return flask.jsonify(success=False, message="'os_install_date' deve seguir o formato 'AAAA-MM-DD HH:MM:SS'"), 400
    
    except Exception as e:
        db.session.rollback()
        log.error(f'new machine error: {e}')
        return flask.jsonify(success=False, message="internal server error"), 500

    return flask.jsonify(machine.dto(versions=True, owner=True))

@machine_bp.route('/machines/<owner_id>')
def get_machines(owner_id:str):
    owner = User.query.get(owner_id)
    if not owner:
        return flask.jsonify(success=False, message="usuário não encontrado!"), 404
    return flask.jsonify(owner.dto(machines=True, versions=True))