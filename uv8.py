# encoding: utf-8
import json
import serial
import logging
# from systemd.journal import JournalHandler
import cv2
import requests
import base64
import time
import os
from flask import Flask, request
import ast
rtsp = 'rtsp://admin:Admin123@192.168.32.205/cam/realmonitor?channel=1&subtype=00&authbasic=YWRtaW46QWRtaW4xMjM='
camout = 'rtsp://admin:Admin123@192.168.32.205/cam/realmonitor?channel=1&subtype=00&authbasic=YWRtaW46QWRtaW4xMjM='
url = 'http://172.17.128.50:58185/api/Farm/postbiohistory'
apitimer = 'http://172.17.128.50:58185/api/Farm/getcountdownsecond'
mac_address = "6c:1c:71:5c:9b:31"  # "6c:1c:71:5c:9b:31" #"6c:1c:71:5b:6d:19"
timer = '0'
status = False
check = False
r = requests.post(apitimer, data=json.dumps(
    {"mac_address": mac_address}), headers={'Content-type': 'application/json', 'Accept': 'text/plain'})
api_timer = r.json()
if api_timer > 0:
    status = True
    check = True
try:
    app = Flask(__name__)
    @app.route('/api/refresh_service', methods = ['POST'])
    def refresh_service():
        content_type = request.headers.get('Content-Type')
        if (content_type == 'application/json'):
            data_uv = ast.literal_eval(request.data.decode())
            with open('test.json', 'w', encoding='utf-8') as out_file:
                json.dump(data_uv, out_file, ensure_ascii=False, indent = 4)  
            
            print(data_uv["phonguv1"])
            timer = (data_uv["timer"])
            if (timer == "90" or timer == "60"):
                cap = cv2.VideoCapture(rtsp)
                retval, img = cap.read()
                strImg64 = base64.b64encode(cv2.imencode('.jpg', img)[1]).decode()
                r = requests.post(url, data=json.dumps({
                    "mac_address": mac_address,
                    "action_name": 'start',
                    "timer": timer,
                    "img": strImg64
                }), headers={
                'Content-type': 'application/json', 'Accept': 'text/plain'})
                file = r.json()
                if file == 200:
                    status = True
                    time.sleep(1)
            # TH2: Neu Status la True va data la 0'
            if (timer == "0"):
                time.sleep(1)
                cap = cv2.VideoCapture(rtsp)
                retval, img = cap.read()
                strImg64 = base64.b64encode(
                    cv2.imencode('.jpg', img)[1]).decode()
                r = requests.post(url, data=json.dumps({
                    "mac_address": mac_address,
                    "action_name": 'end',
                    "timer": timer,
                    "img": strImg64
                }), headers={'Content-type': 'application/json',
                                'Accept': 'text/plain'})
                file = r.json()
                if file == 200:
                    status = False
                    time.sleep(1)
            # TH3: Neu check = True add data = checkout 
            # if not check and (data == "checkout"):
            # if not status and data == "checkout":
            #     time.sleep(3)
            #     cap = cv2.VideoCapture(rtsp)
            #     retval, img = cap.read()
            #     strImg64 = base64.b64encode(
            #         cv2.imencode('.jpg', img)[1]).decode()
            #     r = requests.post(url, data=json.dumps({
            #         "mac_address": mac_address,
            #         "action_name": 'RECEIVE',
            #         "timer": '',
            #         "img": strImg64
            #     }), headers={'Content-type': 'application/json',
            #                     'Accept': 'text/plain'})
            #     file = r.json()
            #     if file == 200:
            #         check = False
            #         time.sleep(1)
                # the json file where the output must be stored
                          
            # out_file.close()
            # with open("test.json", "w") as write_file:
            #     json.dumps(json_data, write_file)
    
            # if (json_data["timer"] == "60"):
            #     print("ok ok ok")
            
            #     json_data = request.data.decode()
            #     json.dump(json_data, write_file)
            return (data_uv)
        else:
            return {'hello': 'sinh'}
    app.run(host='0.0.0.0', port=58888)
    
    
except:
    pass