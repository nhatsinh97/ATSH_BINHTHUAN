#!/usr/bin/env python
import configparser
#import xml.etree.ElementTree as ET
import lxml.etree as ET
from io import StringIO, BytesIO
from shutil import copyfile
import requests
from requests.auth import HTTPDigestAuth
from subprocess import Popen
import serial
# from picamera import PiCamera
# from picamera.array import PiRGBArray
import logging
from systemd.journal import JournalHandler
import cv2
import json
import base64
from PIL import Image
import time
import datetime
import os
import argparse
import sys
import psutil
from libs.object_detector import ObjectDetector
from libs.object_detector import ObjectDetectorOptions
from libs.utils import visualize
import numpy as np

rtsp = 'rtsp://admin:Admin123@192.168.32.205/cam/realmonitor?channel=1&subtype=00&authbasic=YWRtaW46QWRtaW4xMjM='
# rtsp = 'rtsp://admin:Admin123@192.168.32.205:554/Streaming/Channels/102'
# rtsp = 'rtsp://admin:Admin123@192.168.32.201/cam/realmonitor?channel=2&subtype=00&authbasic=YWRtaW46QWRtaW4xMjM='
# rtsp = PiCamera()
# rtsp.start_preview()
# rtsp = 0
camout = 'rtsp://admin:Admin123@192.168.32.204/cam/realmonitor?channel=1&subtype=00&authbasic=YWRtaW46QWRtaW4xMjM='
# url = 'http://172.16.128.41:8089/api/Farm/postbiohistory'
# apitimer = 'http://172.16.128.41:8089/api/Farm/getcountdownsecond'
url = 'http://172.17.128.50:8089/api/Farm/postbiohistory'
apitimer = 'http://172.17.128.50:8089/api/Farm/getcountdownsecond'
mac_address = "6c:1c:71:5c:9b:31"   # "6c:1c:71:5c:9b:31" #"6c:1c:71:5b:6d:19"

NVR_URL = 'http://192.168.32.205'
NVR_USR = 'admin'
NVR_PASS = 'Admin123'

urlAlertStream = NVR_URL + '/ISAPI/Event/notification/alertStream'
urlPicture = NVR_URL + '/ISAPI/Streaming/channels/%d/picture'
parse_string = ''
start_event = False


XML_NAMESPACE = 'http://www.hikvision.com/ver20/XMLSchema'

DEFAULT_HEADERS = {
    'Content-Type': "application/xml; charset='UTF-8'",
    'Accept': "*/*"
}

hik_request = requests.Session()
hik_request.auth = HTTPDigestAuth(NVR_USR, NVR_PASS)
hik_request.headers.update(DEFAULT_HEADERS)


# define the countdown func.
def countdown(t):
    while t > 0:
        mins, secs = divmod(t, 60)
        # timeformat = '{:02d}:{:02d}'.format(mins, secs)
        # print(timeformat, end='\r')
        time.sleep(1)
        t -= 1
        return t
    return countdown(t-1)

def temperature_of_raspberry_pi():
    cpu_temp = os.popen("vcgencmd measure_temp").readline()
    return cpu_temp.replace("temp=", "")

def is_valid_image(image):
    # Convert image to HSV color space
    image = cv2.cvtColor(image, cv2.COLOR_BGR2HSV)

    # Calculate histogram of saturation channel
    s = cv2.calcHist([image], [1], None, [256], [0, 256])

    # Calculate percentage of pixels with saturation >= p
    p = 0.05
    s_perc = np.sum(s[int(p * 255):-1]) / np.prod(image.shape[0:2])

    # Percentage threshold; above: valid image, below: noise
    s_thr = 0.5
    return s_perc > s_thr

def callHik(start_event=False, parse_string='', status=False, count=0, time_countdown=0):
    log.info("===START === callHik")
    stream = hik_request.get(urlAlertStream, stream=True, timeout=(5, 60), verify=False)
    if stream.status_code != requests.codes.ok:
        print("Can't connect to the stream!")
        # raise ValueError('Connection unsuccessful.')
    else:
        # fail_count = 0
        human_detected = False
        # count = 1
        log.info('Connection successful to: ' + NVR_URL)

        for line in stream.iter_lines():
            # filter out keep-alive new lines
            if line:
                str_line = line.decode("utf-8")
                if str_line.find('<EventNotificationAlert') != -1:
                    start_event = True
                    parse_string += str_line
                elif str_line.find('</EventNotificationAlert>') != -1:
                    parse_string += str_line
                    start_event = False
                    if parse_string:
                        #tree = ET.fromstring(parse_string)
                        # Use lxml instead of xml
                        parser = ET.XMLParser(recover=True)                       
                        tree = ET.parse(StringIO(parse_string), parser=parser)

                        channelID = tree.find('{%s}%s' % (XML_NAMESPACE, 'channelID'))
                        if channelID is None:
                            # Some devices use a different key
                            channelID = tree.find('{%s}%s' % (XML_NAMESPACE, 'dynChannelID'))

                        if channelID.text == '0':
                            # Continue and clear the chunk
                            parse_string = ""
                            continue

                        eventType = tree.find('{%s}%s' % (XML_NAMESPACE, 'eventType'))
                        eventState = tree.find('{%s}%s' % (XML_NAMESPACE, 'eventState'))
                        postCount = tree.find('{%s}%s' % (XML_NAMESPACE, 'activePostCount'))

                        current_date = datetime.datetime.now().strftime("%Y-%m-%d %H:%M:%S")
                        log.info('===INITIAL time_countdown === %d', time_countdown)
                        while time_countdown:
                            mins, secs = divmod(time_countdown, 60)
                            timeformat = '{:02d}:{:02d}'.format(mins, secs)
                            print(timeformat, end='\r')
                            time.sleep(1)
                            time_countdown -= 1
                            log.info('===UPDATE time_countdown === %d', time_countdown)
                            if time_countdown <= 0:
                                return status, 0
                        log.info('=== eventType.text === %s', str(eventType.text))
                        if eventType.text == 'VMD':
                            log.info('=== human_detected === %s', str(human_detected))
                            log.info('=== eventType.text === %s', str(eventType.text))
                        if not human_detected and eventType.text == 'VMD' and count < 10:
                            stream2 = hik_request.get(urlPicture % int(channelID.text), stream=True, timeout=(5, 60), verify=False)  
                            if stream2.status_code != requests.codes.ok:
                                print("Can't connect to the stream!")
                                # raise ValueError('Connection unsuccessful.')
                            else:
                                strImg64 = base64.b64encode(stream2.content).decode()
                                r = requests.post(url, data=json.dumps({
                                        "mac_address": mac_address,
                                        "action_name": 'HUMAN_DETECT',
                                        "timer": '',
                                        "img": strImg64
                                    }), headers={
                                        'Content-type': 'application/json', 'Accept': 'text/plain'})
                                file = r.json()
                                if file == 200:
                                    time.sleep(1)
                                    status = True
                                count +=1
                                # if strImg64:
                                #     img = Image.open(BytesIO(stream2.content))
                                #     imagePath = ('D:\\workspace\\MyCompany\\ATSH_BINHTHUAN\\test%d.jpeg' % count)
                                #     print('========imagePath=========== ' + imagePath)
                                #     img.save(imagePath, 'jpeg')

                                time.sleep(2)
                            human_detected = True
                        if human_detected and eventType.text == 'videoloss' and count < 10:
                            human_detected = False

                        # Clear the chunk
                        parse_string = ""
                else:
                    if start_event:
                        parse_string += str_line
    log.info("===END === callHik")
    return status, 0


def run(model: str, camera_id: str, width: int, height: int, num_threads: int,
        enable_edgetpu: bool) -> None:
    """Continuously run inference on images acquired from the camera.

    Args:
    model: Name of the TFLite object detection model.
    camera_id: The camera id to be passed to OpenCV.
    width: The width of the frame captured from the camera.
    height: The height of the frame captured from the camera.
    num_threads: The number of CPU threads to run the model.
    enable_edgetpu: True/False whether the model is a EdgeTPU model.
    """
    print("run")
    try:
        timer = '0'
        status = False
        check = False
        # Khai bao chi tiet cac thuoc tinh cua Port noi tiep
        r = requests.post(apitimer, data=json.dumps(
            {"mac_address": mac_address}), headers={'Content-type': 'application/json', 'Accept': 'text/plain'})
        time_countdown = r.json()
        print("time_countdown" + str(time_countdown))
        if time_countdown > 0:
            status = True
            check = True
            time.sleep(1)
        ser = serial.Serial(port='/dev/ttyS0', baudrate=115200, parity=serial.PARITY_NONE,
                            stopbits=serial.STOPBITS_ONE, bytesize=serial.EIGHTBITS, timeout=1)
        print("run---ser")
        # Variables to calculate FPS
        counter, fps = 0, 0
        start_time = time.time()
        
        # Visualization parameters
        row_size = 20  # pixels
        left_margin = 24  # pixels
        text_color = (0, 0, 255)  # red
        font_size = 1
        font_thickness = 1
        fps_avg_frame_count = 10

        # Initialize the object detection model
        options = ObjectDetectorOptions(
            num_threads=num_threads,
            score_threshold=0.3,
            max_results=3,
            enable_edgetpu=enable_edgetpu)
        detector = ObjectDetector(model_path=model, options=options)
        count_human_defection = 0
        
        while True:
            # Getting loadover15 minutes
            load1, load5, load15 = psutil.getloadavg()
            cpu_usage = (load15/os.cpu_count()) * 100
            print("The CPU usage is : ", cpu_usage)
            # Getting % usage of virtual_memory ( 3rd field)
            # print('RAM memory % used:', psutil.virtual_memory()[2])
            # print(temperature_of_raspberry_pi())
		
            s = ser.readline()  # Cho doi (timeout) de doc du lieu tu Port noi tiep
            data = s.decode()  # Giai ma chuoi du lieu
            data = data.rstrip()  # Loai bo “\r\n” o cuoi chuoi du lieu
            data = data.rstrip()
            log.info("==temperature=======%s=", temperature_of_raspberry_pi())
            log.info("==data===get from MACH DIEN====%s=", data)
            log.info("==status======%s=", str(status))
            # print(data)
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
                log.info("==start======%s=", str(file))
                if file == 200:

                    status = True
                    time.sleep(1)
            # TH2: Neu Status la True
            # if status:
            #     # va data la 0'
            log.info("=check=time_countdown=%s", str(time_countdown))
            if status:
                if data == "0":
                    # print("=====Lenh end============")
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
                    log.info("==end======%s=", str(file))
                    if file == 200:
                        status = False
                        time.sleep(1)
                # print("=====Count dơn >0 ============", time_countdown)
                # if time_countdown <= 0:
                #     log.info("=check=time_countdown=%s", str(time_countdown))
                #     break
                if time_countdown > 0:
                    try:
                        fail_count = 0             
                        status, time_count = callHik(start_event=start_event, parse_string=parse_string, status=status, count=0, time_countdown=time_countdown)
                        # log.info("==status==time_count==***%d****==", time_count )
                        if time_count == 0:
                            log.info("====BREAK==*******==")
                            break
                    except (ValueError, requests.exceptions.ConnectionError, requests.exceptions.ChunkedEncodingError) as err:
                        fail_count += 1
                        time.sleep(fail_count * 5)
                        log.error("====ValueError====" + str(fail_count))
                        continue
            # TH3: Neu check = True add data = checkout 
            # if not check and (data == "checkout"):
            if data == "checkout":
                time.sleep(1)
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
                if file == 200:
                    check = False
                    time.sleep(1)
            #___________________________________________

    except KeyboardInterrupt:
        ser.close()  # Dong Port noi tiep
        # print("có lỗi")
    except TypeError as error:
        #os.system("sudo reboot")
        log.error(error)
    except AssertionError as error:
        print(error)
    # except:
    #     print("cos loi")
    #     os.system("sudo reboot")
    #     pass


def main():
    parser = argparse.ArgumentParser(
        formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument(
        '--model',
        help='Path of the object detection model.',
        required=False,
        default='libs/efficientdet_lite0.tflite')
    parser.add_argument(
        '--cameraId', help='Id of camera.', required=False, type=str, default=rtsp)
    parser.add_argument(
        '--frameWidth',
        help='Width of frame to capture from camera.',
        required=False,
        type=int,
        default=320)
    parser.add_argument(
        '--frameHeight',
        help='Height of frame to capture from camera.',
        required=False,
        type=int,
        default=240)
    parser.add_argument(
        '--numThreads',
        help='Number of CPU threads to run the model.',
        required=False,
        type=int,
        default=2)
    parser.add_argument(
        '--enableEdgeTPU',
        help='Whether to run the model on EdgeTPU.',
        action='store_true',
        required=False,
        default=False)
    args = parser.parse_args()
    # Set time is 5s
    time.sleep(5)

    run(args.model, args.cameraId, args.frameWidth, args.frameHeight,
        int(args.numThreads), bool(args.enableEdgeTPU))


if __name__ == '__main__':
    log = logging.getLogger('UV TEST')
    log.addHandler(JournalHandler())
    log.setLevel(logging.INFO)
    log.info("====BEGIN==== UV ========")
    main()
