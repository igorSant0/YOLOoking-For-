from flask import Blueprint, Response
import cv2
import os
from core.config.settings import CAMERA_URL

from core.state import detector

web_bp = Blueprint('web', __name__)

def generate_yolo_frames():
    cap = cv2.VideoCapture(CAMERA_URL)
    
    while True:
        success, frame = cap.read()
        if not success:
            break
            
        annotated_frame = detector.process_frame(frame) 
        
        ret, buffer = cv2.imencode('.jpg', annotated_frame)
        frame_bytes = buffer.tobytes()
        
        yield (b'--frame\r\n'
               b'Content-Type: image/jpeg\r\n\r\n' + frame_bytes + b'\r\n')

@web_bp.route('/')
def home():
    return "YOLOuking For Server Active and Modularized!"

@web_bp.route('/yolo-live')
def yolo_live():
    return Response(generate_yolo_frames(), mimetype='multipart/x-mixed-replace; boundary=frame')