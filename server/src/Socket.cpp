#include <cstdint>
#include <iomanip>
#include <netinet/in.h>
#include <sstream>
#include <spdlog/spdlog.h>
#include <stdexcept>
#include <string>
#include <sys/socket.h>

#include "../lib/CRC.h"
#include "Socket.h"

using namespace std;

namespace Socket {

static uint32_t calculateChecksum(uint32_t len, void* bytes)
{
    return CRC::Calculate(bytes, len, CRC::CRC_32());
}

static string toReadableHex(const uint32_t len, const uint8_t* buffer)
{
    stringstream hexstr;

    for (uint32_t i = 0; i < len; i++) {
        hexstr << hex << setw(2) << setfill('0') << static_cast<int>(buffer[i]) << " ";
    }

    return hexstr.str();
}

class RequestPackage : public Package
{
    private:
        struct Header {
            const uint32_t checksum;
            const uint16_t version;
            const uint16_t contentType;
            const uint32_t contentLen;
        };

        const uint32_t len;
        const uint8_t* buffer;
        const Header* header;

        void assertConsistency()
        {
            if (len < sizeof(Header)) {
                throw runtime_error("Package length too short");
            }

            const uint32_t receivedContentLen = len - sizeof(Header);
            const uint32_t receivedChecksum = calculateChecksum(len - sizeof(uint32_t), (void*) (buffer + sizeof(uint32_t)));

            if (header->contentLen != receivedContentLen) {
                throw runtime_error("Package content length mismatch");
            }

            if (header->checksum != receivedChecksum) {
                throw runtime_error("Package checksum mismatch");
            }
        }

    public:
        RequestPackage(const uint32_t len, const uint8_t* buffer) :
            len(len),
            buffer(buffer),
            header((Header*) buffer)
        {
            assertConsistency();
        }

        uint16_t contentType() override
        {
            return header->contentType;
        }

        uint32_t contentLen() override
        {
            return header->contentLen;
        }

        uint8_t* content() override
        {
            return (uint8_t*) (buffer + sizeof(Header));
        }
};

class CSocket : public Socket
{
    private:
        const int port;
        int serverSocket;
        int clientSocket;

        void createSocket() 
        {
            serverSocket = ::socket(AF_INET, SOCK_STREAM, 0);

            if (serverSocket < 0) {
                throw runtime_error("Error creating socket");
            }

            spdlog::debug("Socket created");
        }

        void bindSocket()
        {
            struct sockaddr_in serverAddr;
            serverAddr.sin_family = AF_INET;
            serverAddr.sin_port = htons(port); 
            serverAddr.sin_addr.s_addr = INADDR_ANY;

            if (::bind(serverSocket, (struct sockaddr*) &serverAddr, sizeof(serverAddr)) < 0) {
                throw runtime_error("Error binding socket");
            }

            spdlog::debug("Socket bound");
        }

        void listenForConnections()
        {
            if (::listen(serverSocket, 3) < 0) {
                throw runtime_error("Error listening");
            }

            spdlog::debug("Listening for connections on port {}", port);
        }

    public:
        CSocket(const int port) :
            port(port)
        {
            createSocket();
            bindSocket();
            listenForConnections();
        }

        void acceptConnection()
        {
            clientSocket = accept(serverSocket, nullptr, nullptr);

            if (clientSocket < 0) {
                throw runtime_error("Error accepting connection");
            }

            spdlog::debug("Client connected");
        }

        void closeConnection()
        {
            ::close(clientSocket);
            spdlog::debug("Client disconnected");
        }

        Package* receiveNextPackage() override
        {
            uint8_t* buffer = new uint8_t[1024];
            int len = recv(clientSocket, buffer, 1024, 0);

            if (len < 0) {
                throw runtime_error("Error receiving package");
            }

            if (len == 0) {
                return nullptr;
            }

            spdlog::debug("Package received: {} ({} bytes)", toReadableHex(len, buffer), len);

            return new RequestPackage(len, buffer);
        }

        void sendNextPackage(string package) override
        {
            if (send(clientSocket, package.c_str(), package.length(), 0) < 0) {
                throw runtime_error("Error sending message");
            }

            spdlog::debug("Sent message to client: {}", package);
        }

        void stopListening() override
        {
            ::close(serverSocket);
            ::close(clientSocket);

            spdlog::debug("Socket closed");
        }

};

Socket* listenTo(const int port)
{
    return new CSocket(port);
}

}