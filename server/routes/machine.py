
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
from sqlalchemy import func

from models.database import User, Machine, db

machine_bp = flask.Blueprint('machine', __name__)

@machine_bp.route('/machine/newMachine', methods=['POST'])
def post_new_machine():
    data = flask.request.get_json()

    try:
        owner = User.query.get_or_404(data['owner_id'], 'Usuário não encontrado')

        os_install_date = data.get('os_install_date')
        if os_install_date:
            try:
                os_install_date = dt.datetime.strptime(os_install_date, '%Y-%m-%d %H:%M:%S')
            except ValueError:
                return flask.jsonify(success=False, message="'os_install_date' deve seguir o formato 'AAAA-MM-DD HH:MM:SS'"), 400


        new_machine = Machine(
            title=data['title'],
            os=data['os'],
            os_architecture=data.get('os_architecture'),
            os_install_date=os_install_date,
            os_version=data.get('os_version'),
            os_serial_number=data.get('os_serial_number'),
            organization=data.get('organization'),
            motherboard_manufacturer=data.get('motherboard_manufacturer'),
            processor=data.get('processor'),
            owner=owner,
        )

        db.session.add(new_machine)
        db.session.commit()
    
    except KeyError:
        return flask.jsonify(success=False, message="'owner_id', 'title' e 'os' devem ser fornecidos!"), 400
    
    except Exception as e:
        db.session.rollback()
        log.error(f'new machine error: {e}')
        return flask.jsonify(success=False, message="internal server error"), 500

    return flask.jsonify(success=True, id=new_machine.id)

@machine_bp.route('/machines/<owner_id>')
def get_machines(owner_id:str):
    owner = User.query.get_or_404(owner_id, 'Usuário não encontrado')
    print(owner)
    
    for i, machine in enumerate(owner.machines):
        print('machine', i, machine)

        for j, version in enumerate(machine.versions):
            print('version', j, version)

        print()
    # return flask.jsonify(
    #     machines=[ m.dto() for m in machines ]
    # )

    data = [
        
    ]

    return flask.jsonify(success=True, data=data)