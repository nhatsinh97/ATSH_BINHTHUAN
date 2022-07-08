# encoding: utf-8
import json
from flask import Flask, request
app = Flask(__name__)
mac = "12345"
data  =  {
        "timer" :  "60" ,
        "img" :  "true"
    }
 
# sử dụng context manager để tạo tệp .json
# with open("test.json", "w") as write_file:
#     json.dump(data, write_file)
#     print(data["timer"])
#     if (data["timer"] == "60"):
#         print("ok ok ok")
@app.route('/api/refresh_service', methods = ['POST'])
def refresh_service():
    content_type = request.headers.get('Content-Type')
    if (content_type == 'application/json'):
        # json = request.json
        # json = json.encode()
        json_data = request.data.decode()
        print(json_data)
        if (data["timer"] == "60"):
            print("ok ok ok")
        # with open("test.json", "w") as write_file:
        #     json_data = request.data.decode()
        #     json.dump(json_data, write_file)
        return (data["timer"])
    else:
        return {'hello': 'mac'}
app.run(host='0.0.0.0', port=58888)