from flask import Flask
from pymongo import MongoClient
from pymongo.errors import ServerSelectionTimeoutError
from dotenv import dotenv_values
import atexit
from flask_cors import CORS
from flask_socketio import SocketIO,emit

def create_app():
    config = dotenv_values(".env")
    app = Flask(__name__)
    
    with app.app_context():
        app.config['SECRET_KEY'] = 'secret!'
        CORS(app,resources={r"/*":{"origins":"*"}})
        socketio = SocketIO(app,cors_allowed_origins="*")

        try:
            app.mongodb_client = MongoClient(config["ATLAS_URI"])
            # app.mongodb_client.admin.command('ismaster')
            app.db = app.mongodb_client[config["DB_NAME"]]
            # print(app.mongodb_client.admin.command('serverStatus'))
        except ServerSelectionTimeoutError as e:
            print("MongoDB connection failed: server is not available") 
    return app, socketio


# def end_server():
#     app.mongodb_client.close()

# app, socket = create_app()
# atexit.register(end_server)