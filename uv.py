import cv2
import requests
import json
import base64
import time
import RPi.GPIO as GPIO
GPIO.setmode(GPIO.BCM)
GPIO.setup(16, GPIO.IN, pull_up_down=GPIO.PUD_UP)    #Nút nhấn 90 phút kết nối với chân GPIO23
GPIO.setup(20, GPIO.IN, pull_up_down=GPIO.PUD_UP)    #Nút nhấn 60 phút kết nối với chân GPIO24

url = 'http://172.16.128.41:8089/api/Farm/postbiohistory'
apitimer = 'http://172.16.128.41:8089/api/Farm/getcountdownsecond'
mac_address = "6c:1c:71:5c:9b:31"   #"6c:1c:71:5c:9b:31" #"6c:1c:71:5b:6d:19"
timer = '60'
try:  
    while True:
        button_start60 = GPIO.input(16)
        button_start90 = GPIO.input(20)
        if button_start60 == 1:
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
            if file > 200:
                time.sleep(60)
            if file == 200:
                t = 3600
                while t:
                    mins, secs = divmod(t, 60)  #tách phút và giây từ biến t
                    timeformat = '{:02d}:{:02d}'.format(mins, secs) #định dạng thời gian hiển thị đếm ngược
                    #print(timeformat, end='\r') #hiển thị thời gian đếm ngược
                    time.sleep(1) # chờ 1s và update thời gian
                    t -= 1  #đếm ngược từng giây cho tới 0
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
                    time.sleep(60) # chờ 1s và update thời gian
except:
    GPIO.cleanup()
    