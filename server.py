from dotenv import load_dotenv
from flask import Flask
import os

from routes.web.web_controller import web_bp
from routes.esp.esp_controller import esp_bp

load_dotenv()
app = Flask(__name__)

app.register_blueprint(web_bp)
app.register_blueprint(esp_bp)

if __name__ == '__main__':
    HOST = os.getenv('HOST', '0.0.0.0')
    PORT = int(os.getenv('PORT', 5001))
    
    print(f"[BOOT] YOLOuking For Server escutando em http://{HOST}:{PORT}", flush=True)
    app.run(host=HOST, port=PORT, debug=True, use_reloader=False)