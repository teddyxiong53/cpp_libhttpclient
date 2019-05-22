#ifndef HTTP_POST_INTERFACE_H
#define HTTP_POST_INTERFACE_H
#include <chrono>
#include <string>
#include <vector>
#include <mutex>
#include <memory>

#include "HTTPResponse.h"

class HttpPostInterface {
public:
    virtual ~HttpPostInterface() = default;
    virtual long doPost(
        const std::string& url,
        const std::string& data,
        std::chrono::seconds timeout,
        std::string& body
    ) = 0;
    virtual HTTPResponse doPost(
        const std::string& url,
        const std::vector<std::string> headerLines,
        const std::vector<std::pair<std::string, std::string>>& data,
        std::chrono::seconds timeout
    ) = 0;
    virtual HTTPResponse doPost(
        const std::string& url,
        const std::vector<std::string> headerLines,
        const std::string& data,
        std::chrono::seconds timeout
    ) = 0;



};

#endif