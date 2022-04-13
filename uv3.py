import serial
import cv2
import requests
import json
import base64
import time
import os
import argparse
import sys
from libs.object_detector import ObjectDetector
from libs.object_detector import ObjectDetectorOptions
from libs.utils import visualize

rtsp = 'rtsp://admin:Admin123@192.168.32.203/cam/realmonitor?channel=1&subtype=00&authbasic=YWRtaW46QWRtaW4xMjM='
camout = 'rtsp://admin:Admin123@192.168.32.204/cam/realmonitor?channel=1&subtype=00&authbasic=YWRtaW46QWRtaW4xMjM='
url = 'http://172.17.128.50:8089/api/Farm/postbiohistory'
apitimer = 'http://172.17.128.50:8089/api/Farm/getcountdownsecond'
mac_address = "6c:1c:71:5c:9b:31"  # "6c:1c:71:5c:9b:31" #"6c:1c:71:5b:6d:19"


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
    try:
        timer = '0'
        status = False
        check = False
        # Khai bao chi tiet cac thuoc tinh cua Port noi tiep
        r = requests.post(apitimer, data=json.dumps(
            {"mac_address": mac_address}), headers={'Content-type': 'application/json', 'Accept': 'text/plain'})
        time_countdown = r.json()
        if time_countdown > 0:
            status = True
            check = True
            time.sleep(1)
        ser = serial.Serial(port='/dev/ttyS0', baudrate=115200, parity=serial.PARITY_NONE,
                            stopbits=serial.STOPBITS_ONE, bytesize=serial.EIGHTBITS, timeout=1)
        ## Test on local
        # time_countdown = 40
        # status = True

        # Variables to calculate FPS
        counter, fps = 0, 0
        start_time = time.time()

        # Start capturing video input from the camera
        cap_detect = cv2.VideoCapture(camera_id)
        cap_detect.set(cv2.CAP_PROP_FRAME_WIDTH, width)
        cap_detect.set(cv2.CAP_PROP_FRAME_HEIGHT, height)

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
    
        while True:
            s = ser.readline()  # Cho doi (timeout) de doc du lieu tu Port noi tiep
            data = s.decode()  # Giai ma chuoi du lieu
            data = data.rstrip()  # Loai bo “\r\n” o cuoi chuoi du lieu
            data = data.rstrip()
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
                if file == 200:
                    status = True
                    time.sleep(1)
            # TH2: Neu Status la True
            if status:
                # va data la 0'
                if data == "0":
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
                if time_countdown > 0:
                    # Continuously capture images from the camera and run inference
                    count_human_defection = 0
                    save_human_defection = False
                    while cap_detect.isOpened():
                        success, image = cap_detect.read()
                        if not success:
                            sys.exit(
                                'ERROR: Unable to read from webcam. Please verify your webcam settings.'
                            )

                        counter += 1
                        image = cv2.flip(image, 1)

                        # Run object detection estimation using the model.
                        detections = detector.detect(image)
                        
                        # Draw keypoints and edges on input image
                        image, is_person = visualize(image, detections)

                        # Calculate the FPS
                        if counter % fps_avg_frame_count == 0:
                            end_time = time.time()
                            fps = fps_avg_frame_count / (end_time - start_time)
                            start_time = time.time()

                        # Show the FPS
                        fps_text = 'FPS = {:.1f}'.format(fps)
                        text_location = (left_margin, row_size)
                        cv2.putText(image, fps_text, text_location, cv2.FONT_HERSHEY_PLAIN,
                                    font_size, text_color, font_thickness)

                        # Stop the program if the ESC key is pressed.
                        if cv2.waitKey(1) == 27:
                            break
                        if is_person and not save_human_defection:
                            time.sleep(5)
                            # cv2.imshow('Warning person!', image)
                            save_human_defection = True
                            count_human_defection += 1
                            print('=========Warning person=======', count_human_defection)
                            # cv2.imwrite('human_detection_%d.jpg' % count_human_defection, image)
                            strImg64 = base64.b64encode(cv2.imencode('.jpg', image)[1]).decode()
                            r = requests.post(url, data=json.dumps({
                                "mac_address": mac_address,
                                "action_name": 'HUMAN_DETECT',
                                "timer": '',
                                "img": strImg64
                            }), headers={
                                'Content-type': 'application/json', 'Accept': 'text/plain'})
                            file = r.json()
                            if file == 200:
                                status = True
                                time.sleep(1)
                        elif not is_person and save_human_defection:
                            save_human_defection = False
                        # else:
                        #     cv2.imshow('object_detector', image)
                    # cap_detect.release()
                    # cv2.destroyAllWindows()

            # TH3: Neu check = True add data = checkout 
            if not check and (data == "checkout"):
                time.sleep(1)
                cap = cv2.VideoCapture(camout)
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
            
    except KeyboardInterrupt:
        ser.close()  # Dong Port noi tiep
        # print("có lỗi")
    except TypeError:
        #os.system("sudo reboot")
        print("có lỗi")
    except:
        pass


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
        default=640)
    parser.add_argument(
        '--frameHeight',
        help='Height of frame to capture from camera.',
        required=False,
        type=int,
        default=480)
    parser.add_argument(
        '--numThreads',
        help='Number of CPU threads to run the model.',
        required=False,
        type=int,
        default=4)
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
    main()
