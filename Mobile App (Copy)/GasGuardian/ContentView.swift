//
//  ContentView.swift
//  GasGuardian
//
//  Created by Pulkith Paruchuri on 4/2/24.
//

import SwiftUI

struct ContentView: View {
    @StateObject var model : ViewModel = ViewModel()
    func getColor(level: Int) -> Color {
        switch(level) {
        case 1: return .yellow
        case 2: return .red
        default: return .green
        }
    }
    
    func getStatus(level: Int) -> String {
        switch(level) {
        case 1: return "CONCERING"
        case 2: return "DANGEROUS"
        default: return "NORMAL"
        }
    }
    
    func getTime(level: Int) -> String {
        switch(level) {
        case 1: return "30 Minutes"
        case 2: return "2 Hours"
        default: return "0 Minutes"
        }
    }
    
    var body: some View {
        VStack {
            Image("aircare-invert")
                .resizable()
                .aspectRatio(contentMode: .fit)
                .frame(height: 50)
            ZStack {
                Circle()
                    .trim(from: 0.5, to: 1)
                    .fill(getColor(level: model.dangerLevel))
                    .scaledToFit()
                    .frame(width: 200, height: 200)
                Circle()
                    .trim(from: 0.5, to: 1)
                    .fill(.black)
                    .scaledToFit()
                    .frame(width: 190, height: 190)
                VStack {
                    Text(String(Int(model.index)))
                        .foregroundColor(.white)
                        .font(.system(size: 60, weight: .bold))
                    Text("Air Quality Index")
                        .foregroundColor(.gray)
                        .font(.system(size: 10, weight: .bold))
                }
                .offset(y: -30)
            }
            
            VStack {
                Text("The air quality is currently")
                Text(getStatus(level: model.dangerLevel))
                    .font(.system(size: 15, weight: .semibold))
                    .foregroundColor(getColor(level: model.dangerLevel))
                    .padding(.bottom, 20)
                
//                .frame(maxWidth: .infinity)
//                .overlay(
//                    Rectangle()
//                        .stroke(Color.yellow)
//                        .cornerRadius(10)
//                )
                HStack {
                    Text("Fan 1: ")
                        .font(.system(size: 18))
                    Text(model.fan1 ? "ENABLED" : "DISABLED")
                        .font(.system(size: 18, weight: .semibold))
                        .foregroundColor(model.fan1 ? .green : .white)
                }
                HStack {
                    Text("Fan 2: ")
                        .font(.system(size: 18))
                    Text(model.fan2 ? "ENABLED" : "DISABLED")
                        .font(.system(size: 18, weight: .semibold))
                        .foregroundColor(model.fan2 ? .green : .white)
                }
                .padding(.bottom, 20)
                Text("Please wait")

                Text(getTime(level: model.dangerLevel))
                    .font(.system(size: 30, weight: .semibold))
                    .foregroundColor(getColor(level: model.dangerLevel))
                Text("Before continuing")
                
            }
            .offset(y: -30)
            .padding(.bottom, 20)
            
            
            VStack {
                Text("If there are high concentrations of a hazardous pollutant, the issue may be excaberated by the current Temperature and Humidity. Take into account the temperature and humidity below. ")
                    .padding(10)
                    .font(.system(size: 12, weight: .semibold))
                Text("Temperature: " + String(Int(model.temp)) + "˚F")
                    .font(.system(size: 20, weight: .heavy))
                    .padding(.bottom, 7 )
                Text("Humidity: " + String(Int(model.humidity)) + "%")
                    .font(.system(size: 20, weight: .heavy))
            }
            .offset(y: -30)
           
            
            
            Spacer()
        }
        .background(Color.black)
    }
}
