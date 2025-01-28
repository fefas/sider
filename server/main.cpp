#include <cstdint>
#include <string.h>

#include "src/Command/Command.h"
#include "src/Logger.h"
#include "src/Socket/Socket.h"
#include "src/Storage/Storage.h"

using namespace std;

// #include <signal.h>
// bool stop = false;
// void signalHandler(int signum) {
//     Logger::debug("Shutting down server");
//     stop = true;
//     exit(0);
// }

int main() {
    SET_DEBUG_LOG_LEVEL;

    LOG_DEBUG("Server starting");
    Socket::Socket* socket = Socket::listenTo(7963);
    Sider::Storage::Storage* storage = Sider::Storage::empty();
    LOG_INFO("Server ready");

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
            case 20: {
                command = Sider::Command::clear(scope, key);
                break;
            }
            case 21: {
                uint8_t* partition = (uint8_t*) (package->content() + end);
                command = Sider::Command::get(scope, key, *partition);
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
                uint8_t* step = (uint8_t*) (package->content() + end);
                uint32_t* ttl = (uint32_t*) (package->content() + end + sizeof(uint8_t));
                command = Sider::Command::count(scope, key, *step, *ttl);
                break;
            }
            case 24: {
                uint8_t* partition = (uint8_t*) (package->content() + end);
                uint8_t* step = (uint8_t*) (package->content() + end + sizeof(uint8_t));
                uint32_t* ttl = (uint32_t*) (package->content() + end + sizeof(uint8_t) + sizeof(uint8_t));
                command = Sider::Command::rate(scope, key, *partition, *step, *ttl);
                break;
            }
            case 30: {
                uint16_t* valueLen = (uint16_t*) (package->content() + end);
                string value = string((char*) (package->content() + end + sizeof(uint16_t)), *valueLen);
                command = Sider::Command::queue(scope, key, value);
                break;
            }
            case 31: {
                command = Sider::Command::dequeue(scope, key);
                break;
            }
            default:
                break;
        }

        Sider::Command::Result result = command->execute(storage);

        socket->sendNextPackage(result.value);
    }

    socket->stopListening();

    return 0;
} 
