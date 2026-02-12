import flask
import logging as log

from models.database import User, db
from tools.utils import validate_cpf

user_bp = flask.Blueprint('user', __name__)

@user_bp.route('/user/newUser', methods=['POST'])
def post_new_user():
    data = flask.request.get_json()

    try:
        cpf = data['cpf']

        if not validate_cpf(cpf):
            return flask.jsonify(success=False, message="CPF deve ser formatado como 000.000.000-00!"), 400

        if User.query.filter_by(cpf=cpf).count():
            return flask.jsonify(success=False, message="Já existe um usuário cadastrado com esse CPF!"), 401

        new_user = User(name=data['name'], cpf=data['cpf'])
        db.session.add(new_user)
        db.session.commit()
    
    except KeyError:
        return flask.jsonify(success=False, message="'cpf' e 'name' devem ser fornecidos!"), 400
    
    except Exception as e:
        db.session.rollback()
        log.error(f'new user error: {e}')
        return flask.jsonify(success=False, message="internal server error"), 500
    
    return flask.jsonify(success=True, id=new_user.id)