from models import Entry, EntryUpdate
from flask import request , jsonify
from bson.objectid import ObjectId
from bson import json_util
import json

class JSONEncoder (json.JSONEncoder):
    def default (self, o):
            if isinstance (o, ObjectId):
                return str(o)
            return json.JSONEncoder.default (self, o)

def jd(data):
    return json.dumps(data, indent=4, default=json_util.default)

def register_routes(app, socket):
    @app.route("/")
    def hello_world():
        return "<p>Hello, World!</p>"
    
    @app.route("/add, methods=['POST']")
    def add_entry():
        data = request.json
        entry = Entry(**data)
        idData = app.db.entries.insert_one(entry.dict())
        id = idData.inserted_id
        # entryMade = app.db.entries.find_one({"_id": ObjectId(id)})
        # print (entryMade)
        data = {
            "status": "OK",
            "id": str(id)
        }
        return jd(data)

    @app.route("/get", methods=['GET'])
    def get_entries():
        entries = list(app.db.entries.find())
        data = {
            "status": "OK",
            "entries": entries
        }
        return jd(data)

    @app.route("/get/<entry_id>", methods=['GET'])
    def get_entry(entry_id):
        entry = app.db.entries.find_one({"_id": ObjectId(entry_id)})
        data = {
            "status": "OK",
            "entry": entry
        }
        return jd(data)
    
    @app.route("/update/<entry_id>", methods=['POST'])
    def update_entry(entry_id):
        data = request.json
        entry = EntryUpdate(**data)
        app.db.entries.update_one({"_id": ObjectId(entry_id)}, {"$set": entry.dict()})
        EntryUpdated = app.db.entries.find_one({"_id": ObjectId(entry_id)})
        data = {
            "status": "OK",
            "entry": str(EntryUpdated["_id"])
        }
        
        sendMobileApp(EntryUpdated);

        return jd(data)
    
    def sendMobileApp(data):
        socket.emit("updatedData", {"data": jd(data)})
    
    @app.route("/forcePush", methods=['GET'])
    def forcePushMobile():
        socket.emit("forcePush", {"data": "forcePush"})
        return jd({"status": "OK"})
