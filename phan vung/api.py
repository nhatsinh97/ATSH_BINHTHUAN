# encoding: utf-8
import json
from flask import Flask, request
app = Flask(__name__)

@app.route('/api/refresh_service', methods = ['POST'])
def refresh_service():
    content_type = request.headers.get('Content-Type')
    print("=============content_type=", content_type)
    print("=============request=", request)
    if (content_type == 'application/json'):
        # json = request.json
        # json = json.encode()
        print("=============mac=", request.data.decode())
        return request.data.decode()
    else:
        return request.data.decode()
    # print("=============mac=", mac)
    # return {'hello': mac}
    # return json.dumps({'name': 'ok'})
    
app.run(host='0.0.0.0', port=58888)