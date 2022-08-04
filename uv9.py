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
cam1 = 'rtsp://admin:Admin123@192.168.32.205/cam/realmonitor?channel=1&subtype=00&authbasic=YWRtaW46QWRtaW4xMjM='
cam2 = 'rtsp://admin:Admin123@192.168.32.202/cam/realmonitor?channel=1&subtype=00&authbasic=YWRtaW46QWRtaW4xMjM='
cam_ruaxe = 'rtsp://admin:Admin123@192.168.32.201/cam/realmonitor?channel=1&subtype=00&authbasic=YWRtaW46QWRtaW4xMjM='
url = 'http://172.17.128.50:58185/api/Farm/postbiohistory'
apitimer = 'http://172.17.128.50:58185/api/Farm/getcountdownsecond'
mac_address = "6c:1c:71:5c:9b:31"  # "6c:1c:71:5c:9b:31" #"6c:1c:71:5b:6d:19"
mac_nharuaxe = "dc:a6:32:0f:bd:ae"
try:
    app = Flask(__name__)
    @app.route('/api/refresh_service', methods = ['POST'])
    def refresh_service():
        content_type = request.headers.get('Content-Type')
        if (content_type == 'application/json'):
            data_uv = ast.literal_eval(request.data.decode())
            backup = (data_uv["backup"])
            if backup == "True":
                with open("./json/total_data.json", "r") as fin:
                    databackup = json.load(fin)
                return (databackup)
            with open("./json/stt.json", "r") as fin:
                stt = json.load(fin)
            stt1 = (stt["stt1"])
            stt2 = (stt["stt2"])
            stt_nrx = (stt["stt_nharuaxe"])
            phonguv1 = (data_uv["phonguv1"])
            phonguv2 = (data_uv["phonguv2"])
            nharuaxe = (data_uv["nharuaxe"])
            status_uv1 = (phonguv1["status_uv1"])
            cb_cua1 = (phonguv1["cb_cua1"])
            status_uv2 = (phonguv2["status_uv2"])
            cb_cua2 = (phonguv2["cb_cua2"])
            
#-------------------------- START XỬ LÝ DATA GỬI LÊN SERVER --------------------------#
            if nharuaxe == "False":
                cap = cv2.VideoCapture(cam2)
                retval, img = cap.read()
                strImg64 = base64.b64encode(cv2.imencode('.jpg', img)[1]).decode()
                r = requests.post(url, data=json.dumps({
                    "mac_address": mac_nharuaxe,
                    "action_name": "end",
                    "timer": "5",
                    "img": strImg64
                }), headers={
                    'Content-type': 'application/json', 'Accept': 'text/plain'})
                file = r.json()
                if file == 200:
                    stt_nrx = "False"
                    data ={ "stt1": stt1 , "stt2": stt2, "stt_nharuaxe" : stt_nrx }
                    with open('./json/stt.json', 'w', encoding='utf-8') as out_file:
                        json.dump(data, out_file, ensure_ascii=False, indent = 4)
            if nharuaxe == "True":
                cap = cv2.VideoCapture(cam_ruaxe)
                retval, img = cap.read()
                strImg64 = base64.b64encode(cv2.imencode('.jpg', img)[1]).decode()
                r = requests.post(url, data=json.dumps({
                    "mac_address": mac_nharuaxe,
                    "action_name": "start",
                    "timer": "5",
                    "img": strImg64
                }), headers={
                    'Content-type': 'application/json', 'Accept': 'text/plain'})
                file = r.json()
                if file == 200:
                    stt_nrx = "True"
                    data ={ "stt1": stt1 , "stt2": stt2, "stt_nharuaxe" : stt_nrx }
                    with open('./json/stt.json', 'w', encoding='utf-8') as out_file:
                        json.dump(data, out_file, ensure_ascii=False, indent = 4)

            #--------1: START XỬ LÝ THÔNG TIN PHÒNG UV1 ------------#
            if stt1 == "False" and  status_uv1 == "1" :
                cap = cv2.VideoCapture(cam1)
                retval, img = cap.read()
                strImg64 = base64.b64encode(cv2.imencode('.jpg', img)[1]).decode()
                r = requests.post(url, data=json.dumps({
                    "mac_address": "6c:1c:71:5c:9b:31",
                    "action_name": "start",
                    "timer": "90",
                    "img": strImg64
                }), headers={
                    'Content-type': 'application/json', 'Accept': 'text/plain'})
                file = r.json()
                if file == 200:
                    stt1 = "True"
                    data ={ "stt1": stt1 , "stt2": stt2, "stt_nharuaxe" : stt_nrx }
                    with open('./json/stt.json', 'w', encoding='utf-8') as out_file:
                        json.dump(data, out_file, ensure_ascii=False, indent = 4)
            if stt1 == "True" and status_uv1 == "0":
                cap = cv2.VideoCapture(cam1)
                retval, img = cap.read()
                strImg64 = base64.b64encode(cv2.imencode('.jpg', img)[1]).decode()
                r = requests.post(url, data=json.dumps({
                    "mac_address": "6c:1c:71:5c:9b:31",
                    "action_name": "end",
                    "timer": "90",
                    "img": strImg64
                }), headers={
                    'Content-type': 'application/json', 'Accept': 'text/plain'})
                file = r.json()
                if file == 200:
                    stt1 = "False"
                    data ={ "stt1": stt1 , "stt2": stt2, "stt_nharuaxe" : stt_nrx }
                    with open('./json/stt.json', 'w', encoding='utf-8') as out_file:
                        json.dump(data, out_file, ensure_ascii=False, indent = 4)

            if cb_cua1 == "1":
                cap = cv2.VideoCapture(cam1)
                retval, img = cap.read()
                strImg64 = base64.b64encode(cv2.imencode('.jpg', img)[1]).decode()
                r = requests.post(url, data=json.dumps({
                "mac_address": "6c:1c:71:5c:9b:31",
                "action_name": 'RECEIVE',
                "timer": '',
                "img": strImg64
                }), headers={'Content-type': 'application/json',
                            'Accept': 'text/plain'})
                file = r.json()
            #--------2: XỬ LÝ THÔNG TIN PHÒNG UV2 ------------#
            if stt2 == "False" and  status_uv2 == "1":
                cap = cv2.VideoCapture(cam2)
                retval, img = cap.read()
                strImg64 = base64.b64encode(cv2.imencode('.jpg', img)[1]).decode()
                r = requests.post(url, data=json.dumps({
                    "mac_address": "6c:1c:71:5b:6d:19",
                    "action_name": "start",
                    "timer": "90",
                    "img": strImg64
                }), headers={
                    'Content-type': 'application/json', 'Accept': 'text/plain'})
                file = r.json()
                if file == 200:
                    stt2 = "True"
                    data ={ "stt1": stt1 , "stt2": stt2, "stt_nharuaxe" : stt_nrx }
                    with open('./json/stt.json', 'w', encoding='utf-8') as out_file:
                        json.dump(data, out_file, ensure_ascii=False, indent = 4)
            
            if stt2 == "True" and  status_uv2 == "0":
                cap = cv2.VideoCapture(cam2)
                retval, img = cap.read()
                strImg64 = base64.b64encode(cv2.imencode('.jpg', img)[1]).decode()
                r = requests.post(url, data=json.dumps({
                    "mac_address": "6c:1c:71:5b:6d:19",
                    "action_name": "end",
                    "timer": "90",
                    "img": strImg64
                }), headers={
                    'Content-type': 'application/json', 'Accept': 'text/plain'})
                file = r.json()
                if file == 200:
                    stt2 = "False"
                    data ={ "stt1": stt1 , "stt2": stt2, "stt_nharuaxe" : stt_nrx }
                    with open('./json/stt.json', 'w', encoding='utf-8') as out_file:
                        json.dump(data, out_file, ensure_ascii=False, indent = 4) 
            if cb_cua2 == "1":
                cap = cv2.VideoCapture(cam2)
                retval, img = cap.read()
                strImg64 = base64.b64encode(cv2.imencode('.jpg', img)[1]).decode()
                r = requests.post(url, data=json.dumps({
                "mac_address": "6c:1c:71:5b:6d:19",
                "action_name": 'RECEIVE',
                "timer": '',
                "img": strImg64
                }), headers={'Content-type': 'application/json',
                            'Accept': 'text/plain'})
                file = r.json()
            with open('./json/total_data.json', 'w', encoding='utf-8') as out_file:
                        json.dump(data_uv, out_file, ensure_ascii=False, indent = 4)
            return (data_uv)
    app.run(host='0.0.0.0', port=58888)  
except:
    pass