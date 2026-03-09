import flask

root_bp = flask.Blueprint('root', __name__)

@root_bp.route('/testConnection')
def test_connection():
    return ''