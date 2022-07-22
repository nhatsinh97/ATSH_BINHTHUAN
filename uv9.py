# encoding: utf-8
import json
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
url = 'http://172.17.128.50:58185/api/Farm/postbiohistory'
apitimer = 'http://172.17.128.50:58185/api/Farm/getcountdownsecond'
mac_address = "6c:1c:71:5c:9b:31"  # "6c:1c:71:5c:9b:31" #"6c:1c:71:5b:6d:19"
try:
    app = Flask(__name__)
    @app.route('/api/refresh_service', methods = ['POST'])
    def refresh_service():
        content_type = request.headers.get('Content-Type')
        if (content_type == 'application/json'):
            # data_uv = request.data.decode()
            data_uv = ast.literal_eval(request.data.decode())
            with open('./json/total_data.json', 'w', encoding='utf-8') as out_file:
                json.dump(data_uv, out_file, ensure_ascii=False, indent = 4) 
            with open("./json/total_data.json", "r") as fin:
                databackup = json.load(fin) 
            return (databackup)
    def postdata(databackup):
        phonguv1 = (databackup["phonguv1"])
        phonguv2 = (databackup["phonguv2"])
        status_uv1 = (phonguv1["status_uv1"])
        cb_cua1 = (phonguv1["cb_cua1"])
        gio1 = (phonguv1["gio1"])
        phut1 = (phonguv1["phut1"])
        giay1 = (phonguv1["giay1"])
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


    app.run(host='0.0.0.0', port=58888)  
except:
    pass