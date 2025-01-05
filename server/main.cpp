#include <cstdint>
// #include <sys/socket.h>
// #include <unistd.h>
#include <string.h>
// #include <signal.h>
// #include <time.h>
// #include <stdexcept>
// #include <unordered_map>
// #include <math.h>
// #include <iomanip>
// #include <sstream>
#include <iostream>
#include <spdlog/spdlog.h>

#include "src/Command/Command.h"
#include "src/Registry/Registry.h"
#include "src/Socket.h"

using namespace std;

// bool stop = false;

// void signalHandler(int signum) {
//     Logger::debug("Shutting down server");
//     stop = true;

//     exit(0);
// }

int main() {
    spdlog::set_level(spdlog::level::debug);
    spdlog::debug("Server starting");

    Socket::Socket* socket = Socket::listenTo(7963);
    Sider::Registry::Registry* registry = Sider::Registry::createEmpty();

    socket->acceptConnection();

    while (true) {
        Socket::Package* package = socket->receiveNextPackage();

        if (package == nullptr) {
            socket->closeConnection();
            socket->acceptConnection();
            continue;
        }

        uint16_t* scopeLen = (uint16_t*) package->content();
        uint16_t* keyLen = (uint16_t*) (package->content() + sizeof(uint16_t));
        string scope = string((char*) (package->content() + sizeof(uint16_t) + sizeof(uint16_t)), *scopeLen);
        string key = string((char*) (package->content() + sizeof(uint16_t) + sizeof(uint16_t) + *scopeLen), *keyLen);
        int end = sizeof(uint16_t) + sizeof(uint16_t) + *scopeLen + *keyLen;

        Sider::Command::Command* command = nullptr;

        switch (package->contentType()) {
            case 21: {
                command = Sider::Command::get(scope, key);
                break;
            }
            case 22: {
                uint32_t* ttl = (uint32_t*) (package->content() + end);
                uint16_t* valueLen = (uint16_t*) (package->content() + end + sizeof(uint32_t));
                string value = string((char*) (package->content() + end + sizeof(uint32_t) + sizeof(uint16_t)), *valueLen);
                command = Sider::Command::keep(scope, key, value, *ttl);
                break;
            }
            case 23: {
                uint32_t* ttl = (uint32_t*) (package->content() + end);
                command = Sider::Command::count(scope, key, *ttl);
                break;
            }
            default:
                break;
        }

        Sider::Command::Result result = command->execute(registry);

        socket->sendNextPackage(result.value);
    }

    socket->stopListening();

    return 0;
} 
