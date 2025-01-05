#ifndef SIDER_SOCKET_H
#define SIDER_SOCKET_H

#include <cstdint>
#include <string>

using namespace std;

namespace Socket
{
    struct Package
    {
    public:
        virtual uint16_t contentType() = 0;
        virtual uint32_t contentLen() = 0;
        virtual uint8_t* content() = 0;
    };

    class Socket
    {
    public:
        virtual Package* receiveNextPackage() = 0;
        virtual void sendNextPackage(string package) = 0;
        virtual void stopListening() = 0;
        virtual void acceptConnection() = 0;
        virtual void closeConnection() = 0;
    };

    Socket* listenTo(const int port);
}

#endif
