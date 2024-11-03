#ifndef CLIENT_H
#define CLIENT_H

#include <string>

class Client {
public:
    void sendMessage(const std::string& message);
    std::string receiveMessage();
};

#endif // CLIENT_H