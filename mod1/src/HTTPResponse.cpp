#include "HTTPResponse.h"

std::string HTTPResponse::serialize() {
    if(0 == code) {
        return "";
    }
    std::string serializedValue = "Code: " + std::to_string(code);
    if(!body.empty()) {
        serializedValue = serializedValue + ",Body: " + body;
    }
    return serializedValue;
}
