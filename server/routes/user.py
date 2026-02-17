import flask
import logging as log

from models.user import User, USER_RULES
from models.database import db
from tools.utils import validate_cpf, token_required

user_bp = flask.Blueprint('user', __name__)

@user_bp.route('/user')
@token_required(return_user_instance=True)
def get_user(logged_user:User):
    return flask.jsonify(logged_user.dto())

@user_bp.route('/user', methods=['POST'])
def post_user():
    data = flask.request.get_json()

    try:
        cpf = data['cpf']
        password = data['password']

        if len(password) < 4:
            return flask.jsonify(message="A senha deve conter no mínimo 4 caracteres"), 401

        if not validate_cpf(cpf):
            return flask.jsonify(message="CPF deve possuir 11 números"), 400

        if User.query.filter_by(cpf=cpf).count():
            return flask.jsonify(message="Já existe um usuário cadastrado com esse CPF!"), 401

        new_user = User(name=data['name'], cpf=data['cpf'])
        new_user.set_password(password)
        db.session.add(new_user)
        db.session.commit()
    
    except KeyError:
        return flask.jsonify(message="'cpf', 'name' e 'password' devem ser fornecidos!"), 400
    
    except Exception as e:
        db.session.rollback()
        log.error(f'new user error: {e}')
        return flask.jsonify(message="internal server error"), 500
    
    return flask.jsonify(id=new_user.id)

@user_bp.route('/user/<ident>')
@token_required(return_user_instance=True)
def get_user_ident(logged_user:User, ident:str):
    # verifica se o usuário autenticado possui permissão
    if not logged_user.check_rule(User.RULE_QUERY_OTHER_USERS):
        return flask.jsonify(message="você não possui autorização para realizar esta requisição"), 401

    user = User.query.filter((User.cpf == ident) | (User.id == ident)).first()

    if not user:
        return flask.jsonify(message="usuário não encontrado!"), 404

    return flask.jsonify(user.dto())

@user_bp.route('/user/updateRules', methods=['POST'])
@token_required(return_user_instance=True)
def post_update_rules(logged_user:User):
    if not logged_user.check_rule(User.RULE_UPDATE_USER_RULES):
        return flask.jsonify(message="você não possui autorização para realizar esta requisição"), 401
    
    data = flask.request.get_json()
    if 'to_add' not in data and 'to_remove' not in data:
        return flask.jsonify(message=f"'to_add', 'to_remove' ou ambos devem ser fornecidos"), 400

    try:
        user = User.query.get(data['user_id']) if 'user_id' in data else logged_user

        if not user:
            return flask.jsonify(message=f'usuário não encontrado'), 404

        rules_to_add = data.get('to_add', [])
        rules_to_remove = data.get('to_remove', [])
        rules = set(rules_to_add + rules_to_remove)

        invalid_rules = ', '.join(filter(lambda x: x not in USER_RULES, rules))
        if invalid_rules:
            return flask.jsonify(message=f'as regras [{invalid_rules}] são desconhecidas'), 400
        
        user.update_rules(to_add=rules_to_add, to_remove=rules_to_remove)
        db.session.commit()
    
    except Exception as e:
        db.session.rollback()
        log.error(f'add rule error: {e}')
        return flask.jsonify(message="internal server error"), 500
    
    return flask.jsonify(user.dto())