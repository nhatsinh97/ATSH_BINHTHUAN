# Giao tiếp Raspberry và tủ điều khiển phòng uv = RS232 #
import cv2
import serial
import requests
import json
import base64
import logging
from systemd.journal import JournalHandler
log = logging.getLogger('Start RS232 to ESP32')
log.addHandler(JournalHandler())
log.setLevel(logging.INFO)
log.info("====BEGIN==== UV ========")
camuv = 'rtsp://admin:Admin123@192.168.32.205/cam/realmonitor?channel=1&subtype=00&authbasic=YWRtaW46QWRtaW4xMjM='
cam = 'rtsp://admin:Admin123@192.168.32.202/cam/realmonitor?channel=1&subtype=00&authbasic=YWRtaW46QWRtaW4xMjM='
biofarm = 'http://172.17.128.50:58185/api/Farm/postbiohistory'
api_timer = 'http://172.17.128.50:58185/api/Farm/getcountdownsecond'
mac_address = "6c:1c:71:5c:9b:31"  # "6c:1c:71:5c:9b:31" #"6c:1c:71:5b:6d:19"
timer = '0'
status = False
check = False
r = requests.post(api_timer, data=json.dumps(
   {"mac_address": mac_address}), headers={'Content-type': 'application/json', 'Accept': 'text/plain'})
data = r.json()
log.info("Data api =%s",data)
if data > 0:
   status = True
   check = True
ser = serial.Serial(port='/dev/ttyUSB0', baudrate=115200, parity=serial.PARITY_NONE, #port='/dev/ttyUSB0'
                            stopbits=serial.STOPBITS_ONE, bytesize=serial.EIGHTBITS, timeout=1)
try:
    while True:
        s = ser.readline()  # Cho doi (timeout) de doc du lieu tu Port noi tiep
        data = s.decode()  # Giai ma chuoi du lieu
        data = data.rstrip()  # Loai bo “\r\n” o cuoi chuoi du lieu
        log.info("Data from ESP32 %s:", data)
        log.info("status %s:", str(status))
        jOject = json.loads(data)
        log.info("Type data json %s:", type(jOject))

except KeyboardInterrupt:
    ser.close()  # Dong Port noi tiep