import serial # Khai bao su dung ham trong thu vien
import cv2
import requests
import json
import base64
import time
rtsp = 'rtsp://admin:Admin123@192.168.32.203/cam/realmonitor?channel=1&subtype=00&authbasic=YWRtaW46QWRtaW4xMjM='
url = 'http://172.16.128.41:8089/api/Farm/postbiohistory'
apitimer = 'http://172.16.128.41:8089/api/Farm/getcountdownsecond'
mac_address = "6c:1c:71:5c:9b:31"   #"6c:1c:71:5c:9b:31" #"6c:1c:71:5b:6d:19"
timer = '0'
starus = False
# Khai bao chi tiet cac thuoc tinh cua Port noi tiep

ser = serial.Serial(port = '/dev/ttyS0', baudrate = 115200, parity = serial.PARITY_NONE,

stopbits = serial.STOPBITS_ONE, bytesize = serial.EIGHTBITS, timeout = 1)
try:
    while True:
        s = ser.readline() # Cho doi (timeout) de doc du lieu tu Port noi tiep
        data = s.decode() # Giai ma chuoi du lieu
        data = data.rstrip() # Loai bo “\r\n” o cuoi chuoi du lieu
        int(data)
        print(data) # Hien thi du lieu nhan duoc
        if data > 3600 & starus == False:
            timer = '60'
            action_name = 'start'
            cap = cv2.VideoCapture('rtsp://admin:Admin123@192.168.32.203/cam/realmonitor?channel=1&subtype=00&authbasic=YWRtaW46QWRtaW4xMjM=')#'rtsp://admin:Admin123@192.168.32.203/cam/realmonitor?channel=1&subtype=00&authbasic=YWRtaW46QWRtaW4xMjM='
            retval, img = cap.read()
            string = base64.b64encode(cv2.imencode('.jpg', img)[1]).decode()
            img_upload = string
            xong = {
              "mac_address": mac_address ,
              "action_name": action_name,
              "timer"      : timer,
              "img"        : img_upload
                 }
            headers = {'Content-type': 'application/json', 'Accept': 'text/plain'}
            r = requests.post(url, data=json.dumps(xong), headers=headers)
            file = r.json()
            if file == 200:
                starus = True
                time.sleep(1)
        if data == 0 & starus == True:
            timer = '60'
            action_name = 'end'
            cap = cv2.VideoCapture('rtsp://admin:Admin123@192.168.32.203/cam/realmonitor?channel=1&subtype=00&authbasic=YWRtaW46QWRtaW4xMjM=')#'rtsp://admin:Admin123@192.168.32.203/cam/realmonitor?channel=1&subtype=00&authbasic=YWRtaW46QWRtaW4xMjM='
            retval, img = cap.read()
            string = base64.b64encode(cv2.imencode('.jpg', img)[1]).decode()
            img_upload = string
            xong = {
              "mac_address": mac_address ,
              "action_name": action_name,
              "timer"      : timer,
              "img"        : img_upload
                 }
            headers = {'Content-type': 'application/json', 'Accept': 'text/plain'}
            r = requests.post(url, data=json.dumps(xong), headers=headers)
            file = r.json()
            if file == 200:
                starus = False
                time.sleep(1)
except KeyboardInterrupt:
    ser.close() # Dong Port noi tiep