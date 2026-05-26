from dotenv import load_dotenv
from flask import Flask
import os

load_dotenv()

app = Flask(__name__)

@app.route('/')
def hello_world():
    return 'Hello, World!'

HOST = os.getenv('HOST')
PORT = os.getenv('PORT')

if __name__ == '__main__':
    app.run(host=HOST, port=PORT, debug=True)