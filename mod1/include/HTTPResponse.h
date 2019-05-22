#ifndef HTTP_RESPONSE_H
#define HTTP_RESPONSE_H
#include <string>

struct HTTPResponse
{
public:
    HTTPResponse()
     :code(0) {

     }
    long code;
    std::string body;
    std::string serialize();
};


#endif