#include "CurlEasyHandleWrapper.h"
#include "HttpPostInterface.h"

class HttpPost : public HttpPostInterface
{
private:
    //单例模式，私有构造函数。
    HttpPost() = default;
    std::string buildPostData(const std::vector<std::pair<std::string, std::string>>& data) const;
    std::mutex m_mutex;
    std::string m_accumulator;//把body累加起来。
    CurlEasyHandleWrapper m_curl;
    static size_t staticWriteCallbackLocked(char *ptr, size_t size, size_t nmemb, void *userdata);

public:
    static std::unique_ptr<HttpPost> create();

    ~HttpPost() = default;
    HttpPost(const HttpPost& rhs) = delete;//删掉拷贝构造函数。
    HttpPost& operator=(const HttpPost& rhs) = delete;
    long doPost(
        const std::string& url,
        const std::string& data,
        std::chrono::seconds timeout,
        std::string& body
    ) override;
    HTTPResponse doPost(
        const std::string& url,
        const std::vector<std::string> headerLines,
        const std::vector<std::pair<std::string, std::string>>& data,
        std::chrono::seconds timeout
    ) override;
    HTTPResponse doPost(
        const std::string& url,
        const std::vector<std::string> headerLines,
        const std::string& data,
        std::chrono::seconds timeout
    ) override;
};
