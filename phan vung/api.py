# encoding: utf-8
import json
from flask import Flask
app = Flask(__name__)

@app.route('/api/refresh_service', methods = ['POST'])
def refresh_service():
    return json.dumps({'name': 'Refresh successfully!'})

app.run(host='0.0.0.0', port=58888)