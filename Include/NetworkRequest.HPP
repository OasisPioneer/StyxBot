#ifndef NETWORK_REQUEST_HPP
#define NETWORK_REQUEST_HPP

#include <string>

#include "LoggingSystem.HPP"

class NetworkRequest
{
public:
    NetworkRequest();
    std::string HTTP_GET(const std::string& URL);
    std::string HTTP_POST(const std::string& URL, const std::string& Data);
    std::string HTTP_POST_JSON(const std::string& URL, const std::string& Data);
    ~NetworkRequest();
private:
    LoggingSystem LOG;
};

#endif // NETWORK_REQUEST_HPP
