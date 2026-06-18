from flask import Blueprint, jsonify
import cv2
import os

from core.state import game, detector
from core.config.settings import CAMERA_URL

esp_bp = Blueprint('esp', __name__)

@esp_bp.route('/game/connect', methods=['GET'])
def connect_hardware():
    game.connect_hardware()
    return jsonify({
        "status": "OK"
    }), 200

@esp_bp.route('/game/init', methods=['GET'])
def init_game():
    if not game.has_connected:
        return jsonify({"status": "BLOCKED"}), 403

    game.start_game()  
    
    current_object = game.get_current_object()
    first_hint = game.request_hint()
    
    return jsonify({
        "status": "STARTED",
        "object_number": game.current_level_index + 1,
        "object_class": current_object["yolo_class"],
        "hint": first_hint,
        "hints_used": game.hints_requested,
        "current_score": game.total_score
    }), 200

@esp_bp.route('/game/validate', methods=['GET'])
def validate_attempt():
    if not game.has_started:
        return jsonify({"status": "BLOCKED"}), 403
    
    target_object = game.get_current_object()
    
    if not target_object:
        return jsonify({"status": "GAME_OVER"}), 400

    if game.hints_requested == 0:
        return jsonify({"status": "BLOCKED"}), 200

    cap = cv2.VideoCapture(CAMERA_URL)
    success, frame = cap.read()
    cap.release()

    if not success:
        return jsonify({"status": "ERROR"}), 500

    results = detector.model(frame, conf=0.5, classes=detector.allowed_classes, verbose=False)
    
    seen_objects = [
        detector.model.names[int(box.cls[0])] 
        for result in results for box in result.boxes
    ]

    print(f"[ESP32] Looking for: {target_object['yolo_class']} | Seeing: {seen_objects}", flush=True)

    if len(seen_objects) == 0:
        return jsonify({
            "status": "NO_DETECTION",
            "hints_used": game.hints_requested,
            "current_score": game.total_score
        }), 200

    target_class = target_object["yolo_class"]

    if target_class in seen_objects:
        points = game.calculate_current_score()
        game.advance_object(is_correct=True)
        return jsonify({
            "status": "CORRECT",
            "points_earned": points,
            "current_score": game.total_score
        }), 200
    else:
        if game.hints_requested < len(target_object["hints"]):
            next_hint = game.request_hint()
            return jsonify({
                "status": "WRONG",
                "hint": next_hint,
                "hints_used": game.hints_requested,
                "current_score": game.total_score
            }), 200
        else:
            game.advance_object(is_correct=False)
            return jsonify({
                "status": "LEVEL_FAILED",
                "points_earned": 0,
                "current_score": game.total_score
            }), 200

@esp_bp.route('/game/next-hint', methods=['GET'])
def get_hint():
    if not game.has_started:
        return jsonify({"status": "BLOCKED"}), 403

    target_object = game.get_current_object()
    
    if not target_object:
        return jsonify({"status": "GAME_OVER"}), 400

    if game.hints_requested >= len(target_object["hints"]):
        return jsonify({
            "status": "EXHAUSTED", 
            "hints_used": game.hints_requested,
            "current_score": game.total_score
        }), 200

    hint_message = game.request_hint()
    
    return jsonify({
        "status": "NEXT_HINT", 
        "hint": hint_message,
        "hints_used": game.hints_requested,
        "current_score": game.total_score
    }), 200

@esp_bp.route('/game/reset', methods=['GET'])
def reset_game():
    game.reset_game()
    return jsonify({"status": "RESET"}), 200