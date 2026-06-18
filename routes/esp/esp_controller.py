from flask import Blueprint, jsonify
import cv2
import os

from core.state import game, detector

from core.config.settings import CAMERA_URL

esp_bp = Blueprint('esp', __name__)

# Rota para inicializar a conexão com o hardware (ESP32), resetar o estado do jogo e preparar para o início.
@esp_bp.route('/game/connect', methods=['GET'])
def connect_hardware():
    game.connect_hardware()
    game.reset_game()
    return jsonify({
        "status": "OK",
    }), 200


# Rota para iniciar o jogo, resetando o estado e fornecendo informações sobre o primeiro objeto a ser identificado.
@esp_bp.route('/game/init', methods=['GET'])
def init_game():
    if not game.has_connected:
        return jsonify({
            "status": "BLOCKED",
            "message": "Aguardando conexao"
        }), 403

    game.start_game()  
    
    current_object = game.get_current_object()
    
    first_hint = game.request_hint()
    
    return jsonify({
        "status": "STARTED",
        "object_number": game.current_level_index + 1,
        "object_class": current_object["yolo_class"],
        "object_name": current_object["display_name"],
        "hint": first_hint,
        "hints_used": game.hints_requested,
        "current_score": game.total_score
    }), 200



@esp_bp.route('/game/validate', methods=['GET'])
def validate_attempt():
    
    if not game.has_started:
        return jsonify({
            "result": "BLOCKED",
            "message": "Inicie o Jogo!"
        }), 403
    
    target_object = game.get_current_object()
    
    if not target_object:
        return jsonify({"error": "Game is inactive"}), 400

    if game.hints_requested == 0:
        return jsonify({
            "result": "BLOCKED",
            "message": "Peca uma dica!" 
        }), 200

    cap = cv2.VideoCapture(CAMERA_URL)
    success, frame = cap.read()
    cap.release()

    if not success:
        return jsonify({"error": "Camera failure"}), 500

    results = detector.model(frame, conf=0.5, classes=detector.allowed_classes, verbose=False)
    
    seen_objects = [
        detector.model.names[int(box.cls[0])] 
        for result in results for box in result.boxes
    ]

    print(f"[ESP32] Looking for: {target_object['yolo_class']} | Seeing: {seen_objects}", flush=True)

    # REGRA 1: Proteção da câmera vazia. 
    # Se o YOLO não viu nada das nossas classes permitidas, não penaliza o jogador.
    if len(seen_objects) == 0:
        return jsonify({
            "result": "NO_DETECTION",
        }), 200

    target_class = target_object["yolo_class"]

    if target_class in seen_objects:
        points = game.calculate_current_score()
        game.advance_object(is_correct=True)
        return jsonify({
            "result": "CORRECT",
            "points_earned": points,
            "total_score": game.total_score
        }), 200
    else:
        # REGRA 2: O YOLO viu objetos, mas nenhum é o correto. Penalidade acionada.
        if game.hints_requested < len(target_object["hints"]):
            # Força o consumo da próxima dica (o que derruba os pontos potenciais)
            next_hint = game.request_hint()
            return jsonify({
                "result": "WRONG",
                "new_hint": next_hint,
                "hints_used": game.hints_requested
            }), 200
        else:
            # Já estava na última dica e errou a tentativa. Fim da linha para este objeto.
            game.advance_object(is_correct=False)
            return jsonify({
                "result": "FAILED",
                "points_earned": 0,
                "total_score": game.total_score
            }), 200
    




# Rota para resetar o jogo
@esp_bp.route('/game/reset', methods=['POST'])
def reset_game():
    game.reset_game()
    return jsonify({"message": "Jogo zerado!"})





# Rota para obter uma dica do objeto atual
@esp_bp.route('/game/hint', methods=['GET'])
def get_hint():
    if not game.has_started:
        return jsonify({
            "hint": "Inicie o Jogo!",
            "hints_used": 0
        }), 403

    hint_message = game.request_hint()
    return jsonify({
        "hint": hint_message,
        "hints_used": game.hints_requested
    })





@esp_bp.route('/game/status', methods=['GET'])
def get_game_status():
    current_object = game.get_current_object()
    
    if not current_object:
        return jsonify({
            "status": "FINISHED",
            "message": "Completaste tudo!", 
            "final_score": game.total_score
        })
    
    return jsonify({
        "status": "STARTED",
        "object_number": game.current_level_index + 1,
        "hints_used": game.hints_requested,
        "current_score": game.total_score
    })