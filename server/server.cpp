#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <stdexcept>
#include <unordered_map>

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

    Logger::setToDebug();
    Socket::startListening(7963);

    ClientMessage clientMessage;

    while (!stop) {
        Socket::acceptConnection();

        do {
            Socket::receiveMessage(clientMessage);

            std::string message(clientMessage.message, clientMessage.length);

            if (message.substr(0, 3) == "SET") {
                size_t firstSpace = message.find(' ', 4);
                std::string key = message.substr(4, firstSpace - 4);
                std::string value = message.substr(firstSpace + 1, message.length() - firstSpace - 2);
                store[key] = value;

                Socket::sendMessage("OK");
            }

            if (message.substr(0, 3) == "GET") {
                size_t firstSpace = message.find(' ', 4);
                std::string key = message.substr(4, message.length() - 5);
                std::string value = store.find(key) != store.end() ? store[key] : "null";

                Logger::debug("Retrieved key=" + key + " value=" + value);
                Socket::sendMessage(value);
            }   
        } while (clientMessage.length > 0);

        Socket::closeConnection();
    }

    Socket::stopListening();
    return 0;
} 