#ifndef NETWORK_REQUEST_HPP
#define NETWORK_REQUEST_HPP

#include <string>

class NetworkRequest
{
public:
    NetworkRequest();
    static std::string HTTP_GET(const std::string& URL);
    static std::string HTTP_POST(const std::string& URL, const std::string& Data);
    static std::string HTTP_POST_JSON(const std::string& URL, const std::string& Data);
    ~NetworkRequest();
};

#endif // NETWORK_REQUEST_HPP
