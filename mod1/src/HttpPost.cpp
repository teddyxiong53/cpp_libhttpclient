#include "HttpPost.h"
#include "mylog.h"
#include <memory>
#include <sstream>

std::unique_ptr<HttpPost> HttpPost::create() {
    std::unique_ptr<HttpPost> httpPost(new HttpPost());

    if(!httpPost) {
        myloge("httpPost is null");
        return nullptr;
    }
    if(httpPost->m_curl.isValid()) {
        return httpPost;
    }
    return nullptr;
}

// 这个是没有header的情况。
// 多了一个body参数。用来放返回的response内容。
// 返回值是long类型。
long HttpPost::doPost(
    const std::string& url,
    const std::string& data,
    std::chrono::seconds timeout,
    std::string& body
)
{
    auto response = doPost(url, {}, data, timeout);
    body = response.body;
    return response.code;
}
// 这个的特点是：data是键值对。这个比较符合我们的使用习惯。
// 函数里会先构造成a=1&b=2这种字符串。
HTTPResponse HttpPost::doPost(
    const std::string& url,
    const std::vector<std::string> headerLines,
    const std::vector<std::pair<std::string, std::string>> &data,
    std::chrono::seconds timeout
)
{
    auto encodedData = buildPostData(data);
    return doPost(url, headerLines, encodedData, timeout);
}

HTTPResponse HttpPost::doPost(
    const std::string& url,
    const std::vector<std::string> headerLines,
    const std::string& data,
    std::chrono::seconds timeout
)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    HTTPResponse response;
    //依次执行if条件里的函数，只要有一个错误，就马上返回。
    /*
    bool ret = false;
    ret |= m_curl.reset();
    mylogd("ret:%d", ret);
    ret |= m_curl.setTransferTimeout(static_cast<long>(timeout.count()));
    mylogd("ret:%d", ret);
    ret |= m_curl.setURL(url);
    mylogd("ret:%d", ret);
    ret |= m_curl.setPostData(data);
    mylogd("ret:%d", ret);
    ret = m_curl.setWriteCallback(staticWriteCallbackLocked, &response.body);
    mylogd("ret:%d", ret);
    */

    if(
        !(m_curl.reset())
        || !m_curl.setTransferTimeout(static_cast<long>(timeout.count()))
        || !m_curl.setURL(url)
        || !m_curl.setPostData(data)
        //|| !m_curl.setWriteCallback(staticWriteCallbackLocked, &response.body)
    )
    {
        myloge("FAIL");
        return HTTPResponse();
    }

    for(auto line: headerLines) {
        if(!m_curl.addHTTPHeader(line)) {
            myloge("addHTTPHeader fail");
            return HTTPResponse();
        }
    }
    auto curlHandle = m_curl.getCurlHandle();
    auto result = curl_easy_perform(curlHandle);
    mylogd("result:%d", result);
    if(result != CURLE_OK) {
        myloge("curl_easy_perform fail");
        return HTTPResponse();
    }
    result = curl_easy_getinfo(curlHandle, CURLINFO_RESPONSE_CODE, &response.code);
    if(result != CURLE_OK) {
        myloge("curl_easy_getinfo fail");
        return HTTPResponse();
    } else {
        return response;
    }

}

std::string HttpPost::buildPostData(const std::vector<std::pair<std::string, std::string>>& data) const {
    std::stringstream dataStream;
    for(auto ix=data.begin(); ix!=data.end(); ix++) {
        if(ix != data.begin()) {
            dataStream << "&";
        }
        dataStream << m_curl.urlEncode(ix->first) << "=" << m_curl.urlEncode(ix->second);
    }
    return dataStream.str();
}

size_t HttpPost::staticWriteCallbackLocked(char *ptr, size_t size, size_t nmemb, void *userdata) {
    if(!userdata) {
        myloge("userdata is null");
        return 0;
    }
    size_t count = size*nmemb;
    auto body = static_cast<std::string*>(userdata);
    body->append(ptr, count);
    return count;
}
