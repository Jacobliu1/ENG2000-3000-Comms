#include <iostream>
#include "json.hpp"
#include <fstream>
#include <string>
#include <cstdlib>
//#include <boost/asio.hpp>

struct Instructions {
    std::string client_type;
    std::string message;
    std::string client_id;
    std::string timestamp;
    std::string status;
    std::string door_status;
    std::string direction;
};

using boost::asio::ip::tcp;

std::string readJsonFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Could not open the file: " << filename << std::endl;
        return "";
    }

    std::string jsonContent((std::istreambuf_iterator<char>(file)),
                             std::istreambuf_iterator<char>());
    file.close();
    return jsonContent;
}

void sendJsonToIP(const std::string& jsonData, const std::string& ip, int port) {
    try {
        boost::asio::io_service io_service;

        // Create a TCP resolver and query for the endpoint
        tcp::resolver resolver(io_service);
        tcp::resolver::query query(ip, std::to_string(port));
        tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);

        // Create a socket and connect to the endpoint
        tcp::socket socket(io_service);
        boost::asio::connect(socket, endpoint_iterator);

        // Send the JSON data
        boost::asio::write(socket, boost::asio::buffer(jsonData));

        std::cout << "JSON data sent to " << ip << ":" << port << std::endl;
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
}

void from_json(const nlohmann::json& j, Instructions& s) {

if(j.contains("client_type")){
    j.at("client_type").get_to(s.client_type);
} else {
    s.client_type = "";  // Assuming client_type is a boolean
}

if(j.contains("message")){
    j.at("message").get_to(s.message);
} else {
    s.message = "";  // Assuming message is a string, so default to an empty string
}

if(j.contains("client_id")){
    j.at("client_id").get_to(s.client_id);
} else {
    s.client_id = "";  // Assuming client_id is an integer, default to -1 or another placeholder
}

if(j.contains("timestamp")){
    j.at("timestamp").get_to(s.timestamp);
} else {
    s.timestamp = "";  // Assuming timestamp is a string, default to an empty string
}

if(j.contains("status")){
    j.at("status").get_to(s.status);
} else {
    s.status = "";  // Assuming status is a string, default to an empty string
}

if(j.contains("door_status")){
    j.at("door_status").get_to(s.door_status);
} else {
    s.door_status = "";  // Assuming door_status is a string, default to an empty string
}

if(j.contains("direction")){
    j.at("direction").get_to(s.direction);
} else {
    s.direction = "";  // Assuming direction is a string, default to an empty string
}
}

using json = nlohmann::json;

int main(){

    short first = 1;
    std::string firstMessage = "AKIN"
    std::ifstream file("STATUS.json");
    //IP of our bladerunner is  10.20.30.101
//connect to wiii
    std::string ssid = "ENGG2K3K";
    std::string password = "";

    // Command to connect to WiFi using nmcli
    std::string command = "nmcli dev wifi connect '" + ssid + "'";
    
    // Execute the command
    int result = system(command.c_str());
    if (result == 0) {
        std::cout << "Successfully connected to " << ssid << std::endl;
    } else {
        std::cerr << "Failed to connect to WiFi" << std::endl;
    }

// connect to wifi

//send json files
std::string filename = "data.json";  // JSON file path
    std::string ip = "10.20.30.1";    // Target IP address
    int port = 3000;          // Target port

    // Read the JSON file
    std::string jsonData = readJsonFile(filename);

    // Check if data was read
    if (jsonData.empty()) {
        std::cerr << "Failed to read JSON file" << std::endl;
        return 1;
    }

    // Send the JSON data to the specified IP address and port
    sendJsonToIP(jsonData, ip, port);
//send..
    
    if (file.is_open()) {
        nlohmann::json j;
        file >> j;
        
        Instructions myInstructions = j.get<Instructions>();
        
        
        std::cout << "Client Type: " << myInstructions.client_type << "\n";
        std::cout << "Message: " << myInstructions.message << "\n";
        std::cout << "Client ID: " << myInstructions.client_id << "\n";
        std::cout << "Timestamp " << myInstructions.timestamp << "\n";
        std::cout << "Status: " << myInstructions.status << "\n";
        std::cout << "Door Status: " << myInstructions.door_status << "\n";
        std::cout << "Direction: " << myInstructions.direction << "\n";
    } else {
        std::cerr << "Could not open file!" << std::endl;
    }



    if(first){
        if(myInstructions.message != "" && myInstructions.message == "AKIN" && 

        }
    }

    

    return 0;
}