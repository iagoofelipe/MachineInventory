import flask
import datetime as dt

from models.user import User
from tools.utils import generate_token, token_required

auth_bp = flask.Blueprint('auth', __name__)

@auth_bp.route('/auth', methods=['POST'])
def post_auth():
    data = flask.request.get_json()

    try:
        cpf = data['cpf']
        password = data['password']

        if not cpf or not password:
            raise ValueError()
        
        user = User.query.filter_by(cpf=cpf).first()
        if not user or not user.check_password(password):
            return flask.jsonify(message="Usuário ou senha incorretos"), 401

    except (KeyError, ValueError):
        return flask.jsonify(message="Campos 'cpf' e 'password' são obrigatórios"), 400
    
    return flask.jsonify(token=generate_token(user.id, dt.timedelta(days=1)))

@auth_bp.route('/auth/validateToken')
@token_required()
def get_validate_token():
    return flask.jsonify(message='Token válido')