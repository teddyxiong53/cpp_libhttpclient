#ifndef CURL_EASY_HANDLE_WARPPER_H
#define CURL_EASY_HANDLE_WARPPER_H

#include <chrono>
#include <curl/curl.h>
#include <string>
#include "mylog.h"
class CurlEasyHandleWrapper {
public:
    using CurlCallback = size_t(*)(char *buffer, size_t blockSize, size_t numBlocks, void *userData);
    using CurlDebugCallback = size_t(*)(char *buffer, curl_infotype infoType, size_t blockSize, size_t numBlocks, void *userData);

    enum class TransferType {
        kGET,
        kPOST,
        kPUT
    };
    CurlEasyHandleWrapper();
    ~CurlEasyHandleWrapper();
    bool reset();
    CURL *getCurlHandle();
    bool isValid();
    bool addHTTPHeader(const std::string& header);
    bool addPostHeader(const std::string& header);
    bool setURL(const std::string& url);
    bool setTransferType(TransferType type);
    bool setPostContent(const std::string& fieldName, const std::string& payload);
    bool setTransferTimeout(const long timeoutSeconds);
    bool addPostStream(const std::string& fieldName, void *userData);
    bool setPostData(const std::string& data);
    bool setConnectionTimeout(const std::chrono::seconds timeoutSeconds);
    bool setWriteCallback(CurlCallback callback, void *userData);
    bool setHeaderCallback(CurlCallback callback, void *userData);
    bool setReadCallback(CurlCallback callback, void *userData);
    bool setDebugCallback(CurlDebugCallback callback, void *userData);

    template <typename ParamType>
    bool setopt(CURLoption option, ParamType type);

    std::string urlEncode(const std::string& in) const;
    long getHTTPResponseCode();
    CURLcode perform();
private:
    bool setDefaultOptions();
    void cleanupResources();
    CURL *m_handle;
    curl_slist *m_requestHeaders;
    curl_slist *m_postHeaders;
    curl_httppost *m_post;
    curl_httppost *m_lastPost;
};

template <typename ParamType>
bool CurlEasyHandleWrapper::setopt(CURLoption option, ParamType type) {
    auto result = CURLE_OK;
    result = curl_easy_setopt(m_handle, option, type);
    if(result != CURLE_OK) {
        myloge("curl_easy_setopt fail: %d\n", result);
        return false;
    }
    return true;
}

#endif
