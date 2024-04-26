//
//  ViewModel.swift
//  GasGuardian
//
//  Created by Pulkith Paruchuri on 4/23/24.
//

import Foundation
import SocketIO

struct dataResponse: Decodable {
    var status: String
    var entries: [dataEntry]

}

struct dataEntry: Decodable {
    var _id: String
    var temperature: Float
    var humidity : Float
    var index : Float
    var fan1 : Bool
    var fan2 : Bool
    var dangerLevel : Int
    var timestamp : String
}



class ViewModel: ObservableObject {
    @Published var temp : Double = 0
    @Published var humidity : Double = 0
    @Published var index : Double = 0
    @Published var fan1 : Bool = false
    @Published var fan2 : Bool = false
    @Published var dangerLevel : Int = 0
    var timer = Timer()
    let manager : SocketManager
    let socket : SocketIOClient
    let server = "https://pulkith.ngrok.app"
        
    var API_KEY = "null"
    var SID = "none";
    
    init() {
        manager = SocketManager(socketURL: URL(string: server)!, config: [.log(false), .compress])
        socket = manager.defaultSocket
        
        connectSocket();
        
        self.getState()
//        self.timer = Timer.scheduledTimer(withTimeInterval: 5, repeats: true, block: { _ in
//            self.getState()
//        })
    
    }
    
    func connectSocket() {
        socket.on(clientEvent: .connect) {data, ack in
            print("socket connected")
            if(data.count > 1) {
                guard let dict = data[1] as? Dictionary<String, Any> else {
                    print("SID Init Failed")
                    return
                }
                if let sid = dict["sid"] {
                    self.SID = sid as? String ?? "none"
                    print("SID Initialized: " + self.SID)
                } else {
                    print("Could not parse SID")
                }
            }
        }
        
        socket.on("updatedData") { response, ack in
            if(response.count > 0) {
                print(response)
                guard let dict = response[0] as? Dictionary<String, Any> else {
                    print("Could Not Parse Data");
                    return
                }
                
                guard let data = dict["data"] else {
                    return
                }
                
                if let parsedData = (data as! String).toJSON() as? [String:AnyObject] {
                    self.setState(data: parsedData)
                }
                
            }
                
//            if let message = data[0] as? String {
//                print("Received message: \(message)")
//            }
        }

        socket.connect()
    }
    
    func getState() {
        Task {
            await fetchState()
        }
    }
    
    func setState(data : [String : Any]) {
            if let temperature = data["temperature"] as? Double {
                self.temp = temperature
            }
            if let humidity = data["humidity"] as? Double {
                self.humidity = humidity
            }
            if let index = data["index"] as? Double {
                self.index = index - 100
//                self.index = 562
            }
            if let fan1 = data["fan1"] as? Bool {
                self.fan1 = fan1
            }
            if let fan2 = data["fan2"] as? Bool {
                self.fan2 = fan2
            }
            if let dl = data["dangerLevel"] as? Int {
                self.dangerLevel = dl
            }
    }
    
    func fetchState() async {
        let query = "/get"
        let full = server + query
        do {
            let fetchedData = try await fetchContact(urlString: full)
            DispatchQueue.main.async {
                // Update all state variables on the main thread
                if let entries = fetchedData["entries"] as? [Any], let firstEntry = entries.first as? [String: Any] {
                    self.setState(data: firstEntry)
                } else {
                    print("Entries not found or in unexpected format")
                }
//                self.temp = fetchedData["temperature"] as! Float
//                self.humidity = fetchedData["humidity"] as! Float
//                self.index = fetchedData["index"] as! Float
//                self.fan1 = fetchedData["fan1"] as! Bool
//                self.fan2 = fetchedData["fan2"] as! Bool
            }
        } catch {
            print("Failed To Fetch")
        }
    }
    
    
    
    func fetchContact(urlString: String) async throws -> [String: Any] {
        guard let url = URL(string: urlString) else {
            throw FetchError.invalidURL
        }
        var request = URLRequest(url: url)
        request.httpMethod = "GET"
//        request.setValue(API_KEY, forHTTPHeaderField: "ACCESS_TOKEN")
        let (data, _) = try await URLSession.shared.data(for: request)
//        if let rawString = String(data: data, encoding: .utf8) {
//            print("Raw response data:\n\(rawString)")
//        }
//        print(err)
//        let result = /*try JSONDecoder().decode(dataInstance.self, from: data)*/
        if let result = try JSONSerialization.jsonObject(with: data) as? [String: Any] {
                        // Now jsonObject is your dictionary and you can use it as needed
//                        print("Fetched dictionary: \(result)")
                        return result
                    } else {
                        print("Data is not a valid JSON dictionary")
                        throw FetchError.invalidResponse
                    }
    }
    enum FetchError: Error {
        case invalidURL
        case missingData
        case invalidResponse
    }
    
    
}

extension String {
    func toJSON() -> Any? {
        guard let data = self.data(using: .utf8, allowLossyConversion: false) else { return nil }
        return try? JSONSerialization.jsonObject(with: data, options: .mutableContainers)
    }
}
