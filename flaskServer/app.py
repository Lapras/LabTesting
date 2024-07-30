from flask import Flask, request, jsonify, send_from_directory, render_template
import platform, os, subprocess
import psycopg2

app = Flask(__name__)


pgpass = os.environ['PGPASS']

from logging.config import dictConfig

dictConfig(
    {
        "version": 1,
        "formatters": {
            "default": {
                "format": "[%(asctime)s] %(levelname)s in %(module)s: %(message)s",
            }
        },
        "handlers": {
            "console": {
                "class": "logging.StreamHandler",
                "stream": "ext://sys.stdout",
                "formatter": "default",
            }
        },
        "root": {"level": "DEBUG", "handlers": ["console"]},
    }
)


def get_db_connection():
    print(pgpass)
    return psycopg2.connect(host='db', database='db', user='COSU', password=pgpass)

os_name = platform.system()
os_vs = platform.version()

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/information', methods=["GET"])
def API():
    return send_from_directory(app.static_folder, 'API.txt')

@app.route('/Send_SQL_Req', methods=['POST'])
def run_SQL():
    input_command = request.json['command']
    if input_command:
        conn = get_db_connection()
        cursor = conn.cursor()
        
        data = cursor.execute(f"SELECT {input_command} FROM stuff;")
        return jsonify(data)
    
if(__name__ == '__main__'):
    app.run(debug=False, port=1234, host ="0.0.0.0")
        
    
