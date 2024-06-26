from flask import Flask, request, jsonify, send_from_directory, render_template
import platform, os, subprocess
import psycopg2

app = Flask(___name___)

def get_db_connection():
    return psycopg2.connect(host='sql', database='stuff', user='postgres', password='coffee')

os_name = platform.system()
os_vs = platform.version()

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/API_Info')
def API_Info():
    return send_from_directory(app.static_folder, 'API.txt')

@app.route('/Send_SQL_Req', methods=['POST'])
def run_SQL():
    input_command = request.json['command']
    if input_command:
        conn = get_db_connection()
        cursor = conn.cursor()
        
        data = cursor.execute("SELECT * FROM stuff where data = %s" (input_command))
        return jsonify(data)
    
if __name__ == '__main__':
    app.run(debug=False, port=1234, host ="0.0.0.0")
        
    