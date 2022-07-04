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
        # data = json.loads(request.json)
        data = request.json
        json = json.dumps(data)
        print(json)
        # json = json.encode()
        # print("=============mac=", request.data.decode())
        # print(data)
        # return data
        # return request.data.decode()
    else:
        return {'hello': mac}
    
app.run(host='0.0.0.0', port=58888)