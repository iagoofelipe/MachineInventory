#############################################################
# SERVER
# ------
# Descrição: Ponto de entrada do servidor Flask
# Data de Criação: 09/02/2026
# Autor: Iago Carvalho
#############################################################

import flask, os, jwt
from flask import jsonify
import datetime as dt

from tools.utils import get_secret_key
from tools.consts import PATH_TEMPLATES, PATH_ROOT, PATH_ASSETS
from models.database import db
from tools.utils import token_required

SECRET_KEY = get_secret_key()
app = flask.Flask(__file__, template_folder=PATH_TEMPLATES, static_folder=PATH_ASSETS, static_url_path='/assets')

@app.route("/")
def home():
    return jsonify(message="Bem-vindo à API segura com JWT!")

@app.route("/login", methods=["POST"])
def login():
    data = flask.request.get_json()

    if not data:
        return jsonify(message="Dados de login não fornecidos!"), 400

    if "username" not in data or "password" not in data:
        return jsonify(message="Campos 'username' e 'password' são obrigatórios!"), 400

    if data["username"] == "admin" and data["password"] == "1234":
        token = jwt.encode(
            {"user": data["username"], "exp": dt.datetime.now(dt.UTC) + dt.timedelta(minutes=30)},
            SECRET_KEY,
            algorithm="HS256"
        )
        return jsonify(token='Bearer ' + token)

    return jsonify(message="Credenciais inválidas!"), 401

@app.route("/protected", methods=["GET"])
@token_required(SECRET_KEY)
def protected(user:str):
    return jsonify(message=f"Bem-vindo, {user}!")

def main():
    app.config['SQLALCHEMY_DATABASE_URI'] = 'sqlite:///' + os.path.join(PATH_ROOT, 'database.db')
    app.config['SQLALCHEMY_TRACK_MODIFICATIONS'] = False
    app.config['SECRET_KEY'] = SECRET_KEY
    
    # app.register_blueprint(apiauth_bp)
    # app.register_blueprint(apiuser_bp)
    # app.register_blueprint(home_bp)
    # app.register_blueprint(login_bp)

    db.init_app(app)

    with app.app_context():
        db.create_all()
        
    app.run(debug=True)


if __name__ == "__main__":
    main()