from flask_socketio import SocketIO, emit
from flask import request, jsonify

def register_socket (socket):
    @socket.on ("connect")
    def connected():
        """event listener when client connects to the server"""
        print(request.sid)
        print("client has connected")
        emit("connect", {"data": f"id: {request.sid}  is connected"})

    @socket.on("data")
    def handle_message(data):
        """event listener when data is recived from the client"""
        print("data from the front end: ",str(data))
        emit("data", {'data': data, 'id': request.sid}, broadcast=True)
    
    @socket.on("disconnect")
    def disconnect():
        """event listener when client disconnects to the server"""
        print("user disconnected")
        emit("disconnect",f"user {request.sid} disconnect", broadcast=True)