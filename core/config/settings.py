import json
import os

DIST_CONFIG = os.path.dirname(os.path.abspath(__file__))

DIST_CORE = os.path.dirname(DIST_CONFIG)
PATH_JSON = os.path.join(DIST_CORE, "data", "objects.json")

with open(PATH_JSON, "r", encoding="utf-8") as arquivo:
    game_objects = json.load(arquivo)

permitted_classes = list(game_objects.keys())

CAMERA_URL = os.getenv("CAMERA_URL", "http://192.168.15.3:8080/video")
