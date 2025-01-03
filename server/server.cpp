#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <stdexcept>
#include <unordered_map>
#include <math.h>
#include <iomanip>
#include <sstream>

class Logger {
private:
    enum Level {
        DEBUG,
        ERROR,
    };

    class Now {
    public:
        static std::string get()
        {
            time_t now = time(0);
            char timestamp[26];
            ctime_r(&now, timestamp);
            timestamp[24] = '\0'; 

            return timestamp;
        }
    };

    static Level reportLevel;

    static void print(Level level, const std::string& message)
    {
        std::string timestamp = Now::get();
        std::string levelString = level == DEBUG ? "DEBUG" : "ERROR";

        std::cout << timestamp << " [" << levelString << "] " << message << std::endl;
    }

public:
    static void debug(const std::string& message)
    {
        if (reportLevel != DEBUG) {
            return;
        }

        print(DEBUG, message);
    }

    static void error(const std::string& message)
    {
        print(ERROR, message);
    }

    static void setToDebug()
    {
        reportLevel = DEBUG;
    }
};

struct ClientMessage {
    char message[1024];
    int length;

    ClientMessage() : length(0) {}
};

class Socket {
private:
    const int port;
    int serverSocket;
    int clientSocket;

    void createSocket() 
    {
        serverSocket = ::socket(AF_INET, SOCK_STREAM, 0);

        if (serverSocket < 0) {
            throw std::runtime_error("Error creating socket");
        }

        Logger::debug("Socket created");
    }

    void bindSocket()
    {
        struct sockaddr_in serverAddr;
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(port); 
        serverAddr.sin_addr.s_addr = INADDR_ANY;

        if (::bind(serverSocket, (struct sockaddr*) &serverAddr, sizeof(serverAddr)) < 0) {
            throw std::runtime_error("Error binding socket");
        }

        Logger::debug("Socket bound");
    }

    void listenForConnections()
    {
        if (::listen(serverSocket, 3) < 0) {
            throw std::runtime_error("Error listening");
        }

        Logger::debug("Listening for connections on port " + std::to_string(port));
    }

    void closeSocket()
    {
        ::close(serverSocket);
        ::close(clientSocket);

        Logger::debug("Socket closed");
    }

    Socket(int port) : port(port)
    {
        createSocket();
        bindSocket();
        listenForConnections();
    }

    static Socket* instance;

public:
    static void startListening(int port)
    {
        if (instance != nullptr) {
            throw std::runtime_error("Socket already listening");
        }

        instance = new Socket(port);
    }

    static void stopListening()
    {
        if (instance == nullptr) {
            throw std::runtime_error("Socket not listening");
        }

        instance->closeSocket();
        delete instance;
        instance = nullptr;
    }

    static void acceptConnection()
    {
        Logger::debug("Accepting connection");

        instance->clientSocket = accept(instance->serverSocket, nullptr, nullptr);

        if (instance->clientSocket < 0) {
            throw std::runtime_error("Error accepting connection");
        }

        Logger::debug("Client connected");
    }

    static void closeConnection()
    {
        close(instance->clientSocket);
        instance->clientSocket = -1;

        Logger::debug("Client disconnected");
    }

    static void receiveMessage(ClientMessage& message)
    {
        message.length = recv(instance->clientSocket, message.message, sizeof(message.message), 0);

        if (message.length < 0) {
            throw std::runtime_error("Error receiving message");
        }

        Logger::debug("Received message from client: " + std::string(message.message, message.length));
    }

    static void sendMessage(const std::string& message)
    {
        if (send(instance->clientSocket, message.c_str(), message.length(), 0) < 0) {
            throw std::runtime_error("Error sending message");
        }

        Logger::debug("Sent message to client: " + message);
    }
};

Logger::Level Logger::reportLevel = Logger::ERROR;
Socket* Socket::instance = nullptr;

bool stop = false;

void signalHandler(int signum) {
    Logger::debug("Shutting down server");
    stop = true;

    exit(0);
}

int main() {
    signal(SIGINT, signalHandler);

    std::unordered_map<std::string, std::string> store;
    std::unordered_map<std::string, int> count;
    std::unordered_map<std::string, int> rateTotal;
    std::unordered_map<std::string, int> ratePart;

    Logger::setToDebug();
    Socket::startListening(7963);

    ClientMessage clientMessage;

    while (!stop) {
        Socket::acceptConnection();

        do {
            Socket::receiveMessage(clientMessage);

            std::string message(clientMessage.message, clientMessage.length);

            if (message.substr(0, 5) == "STORE") {
                size_t asPos = message.find(" AS ");
                std::string key = message.substr(6, asPos - 6);
                std::string value = message.substr(asPos + 4, message.length() - asPos - 5);
                store[key] = value;

                Socket::sendMessage("OK");
            }

            if (message.substr(0, 5) == "COUNT") {
                std::string key = message.substr(6, message.length() - 7);
                count[key]++;

                Socket::sendMessage("OK");
            }

            if (message.substr(0, 4) == "RATE") {
                if (message.find("PART") != std::string::npos) {
                    std::string key = message.substr(5, message.length() - 11);
                    rateTotal[key]++;
                    ratePart[key]++;
                } else {
                    std::string key = message.substr(5, message.length() - 12);
                    rateTotal[key]++;
                }

                Socket::sendMessage("OK");
            }

            if (message.substr(0, 3) == "GET") {
                size_t firstSpace = message.find(' ', 4);
                std::string key = message.substr(4, message.length() - 5);

                if (store.find(key) != store.end()) {
                    Socket::sendMessage(store[key]);
                } else if (count.find(key) != count.end()) {
                    Socket::sendMessage(std::to_string(count[key]));
                } else if (rateTotal.find(key) != rateTotal.end()) {
                    int rate = (ratePart[key] * 100) / rateTotal[key];
                    float rateFloat = roundf(rate) / 100;

                    std::stringstream stream;
                    stream << std::fixed << std::setprecision(2) << rateFloat;
                    std::string s = stream.str();
                    Socket::sendMessage(s);
                } else {
                    Socket::sendMessage("null");
                }
            }   
        } while (clientMessage.length > 0);

        Socket::closeConnection();
    }

    Socket::stopListening();
    return 0;
} 