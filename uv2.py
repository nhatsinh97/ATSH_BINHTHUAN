import serial
import logging
from systemd.journal import JournalHandler
import cv2
import requests
import json
import base64
import time
import os
time.sleep(5)
log = logging.getLogger('UV TEST')
log.addHandler(JournalHandler())
log.setLevel(logging.INFO)
log.info("====BEGIN==== UV ========")
rtsp = 'rtsp://admin:Admin123@192.168.32.205/cam/realmonitor?channel=1&subtype=00&authbasic=YWRtaW46QWRtaW4xMjM='
camout = 'rtsp://admin:Admin123@192.168.32.205/cam/realmonitor?channel=1&subtype=00&authbasic=YWRtaW46QWRtaW4xMjM='
url = 'http://172.17.128.50:58185/api/Farm/postbiohistory'
apitimer = 'http://172.17.128.50:58185/api/Farm/getcountdownsecond'
mac_address = "6c:1c:71:5c:9b:31"  # "6c:1c:71:5c:9b:31" #"6c:1c:71:5b:6d:19"
timer = '0'
status = False
#status = True
check = False
# Khai bao chi tiet cac thuoc tinh cua Port noi tiep
r = requests.post(apitimer, data=json.dumps(
   {"mac_address": mac_address}), headers={'Content-type': 'application/json', 'Accept': 'text/plain'})
data = r.json()
# print(data)
log.info("==data===api====%s=",data)
if data > 0:
   status = True
   check = True
   time.sleep(1) #/dev/ttyS0 /dev/ttyAMA0
ser = serial.Serial(port='/dev/ttyUSB0', baudrate=115200, parity=serial.PARITY_NONE,
                            stopbits=serial.STOPBITS_ONE, bytesize=serial.EIGHTBITS, timeout=1)
try:
    while True:
        s = ser.readline()  # Cho doi (timeout) de doc du lieu tu Port noi tiep
        data = s.decode()  # Giai ma chuoi du lieu
        data = data.rstrip()  # Loai bo ā\r\nā o cuoi chuoi du lieu
        # print(data)
        log.info("==data===get from MACH DIEN====%s=", data)
        log.info("==status======%s=", str(status))
        # TH1: Neu Status la False va data la 90' hoac 60'
        if not status and (data == "90" or data == "60"):
            timer = '60'
            if data == "90":
                timer = '90'

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
            log.info("==data===start====%s=",file)
            if file == 200:
                status = True
                time.sleep(1)
        # TH2: Neu Status la True va data la 0'
        if status and data == "0":
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
            log.info("==data===end====%s=",file)
            if file == 200:
                status = False
                time.sleep(1)
        # TH3: Neu check = True add data = checkout 
        # if not check and (data == "checkout"):
        if not status and data == "checkout":
            log.info("Bįŗ®T Äįŗ¦U Gį»¬I")
            time.sleep(3)
            log.info("ÄANG Gį»¬I DATA")
            cap = cv2.VideoCapture(rtsp)
            retval, img = cap.read()
            strImg64 = base64.b64encode(
                cv2.imencode('.jpg', img)[1]).decode()
            r = requests.post(url, data=json.dumps({
                "mac_address": mac_address,
                "action_name": 'RECEIVE',
                "timer": '',
                "img": strImg64
            }), headers={'Content-type': 'application/json',
                            'Accept': 'text/plain'})
            file = r.json()
            log.info("==data===checkout====%s=",file)
            if file == 200:
                check = False
                time.sleep(1)
        #___________________________________________
        # TH4: Neu check = True add data = RECEIVE 
        if status and data == "RECEIVE":
            # time.sleep(10)
            cap = cv2.VideoCapture(rtsp)
            retval, img = cap.read()
            strImg64 = base64.b64encode(
                cv2.imencode('.jpg', img)[1]).decode()
            r = requests.post(url, data=json.dumps({
                "mac_address": mac_address,
                "action_name": 'HUMAN_DETECT',
                "timer": '',
                "img": strImg64
            }), headers={'Content-type': 'application/json',
                            'Accept': 'text/plain'})
            file = r.json()
            log.info("==data===RECEIVE====%s=",file)
            if file == 200:
                # data = ""
                check = False
                time.sleep(1)    
            time.sleep(10)
except KeyboardInterrupt:
    ser.close()  # Dong Port noi tiep

# except TypeError:
#     #os.system("sudo reboot")
#     print("cĆ³ lį»i")
# except:
#     pass
