#include "CurlEasyHandleWrapper.h"
#include "libcurlUtils.h"
#include "mylog.h"

static std::string JSON_MIME_TYPE = "text/json";
/// MIME Content-Type for octet stream data
static std::string OCTET_MIME_TYPE = "application/octet-stream";

CurlEasyHandleWrapper::CurlEasyHandleWrapper()
 : m_handle{curl_easy_init()},
   m_requestHeaders{nullptr},
   m_postHeaders{nullptr},
   m_post{nullptr},
   m_lastPost{nullptr}
{
    if(m_handle == nullptr) {
        mylogd("CurlEasyHandleWrapper m_handle is null\n");
    } else {
        setDefaultOptions();
    }
}

CurlEasyHandleWrapper::~CurlEasyHandleWrapper() {
    cleanupResources();
    if(m_handle) {
        curl_easy_cleanup(m_handle);
    }
}

bool CurlEasyHandleWrapper::isValid() {
    return m_handle != nullptr;
}

CURL* CurlEasyHandleWrapper::getCurlHandle() {
    return m_handle;
}

bool CurlEasyHandleWrapper::addHTTPHeader(const std::string& header) {
    m_requestHeaders = curl_slist_append(m_requestHeaders, header.c_str());
    if(!m_requestHeaders) {
        mylogd("curl_slist_append fail\n");
        return false;
    }
    return setopt(CURLOPT_HTTPHEADER, m_requestHeaders);
}
bool CurlEasyHandleWrapper::addPostHeader(const std::string& header) {
    m_postHeaders = curl_slist_append(m_postHeaders, header.c_str());
    if(!m_postHeaders) {
        mylogd("curl_slist_append fail\n");
        return false;
    }
    return true;
    //不存在这个选项
    //return setopt(CURLOPT_POSTHEADER, m_postHeaders);
}

bool CurlEasyHandleWrapper::setURL(const std::string& url) {
    return setopt(CURLOPT_URL, url.c_str());
}

bool CurlEasyHandleWrapper::setTransferType(TransferType type) {
    bool ret = false;
    switch(type) {
        case TransferType::kGET:
            ret = setopt(CURLOPT_HTTPGET, 1L);
            break;
        case TransferType::kPOST:
            ret = setopt(CURLOPT_HTTPPOST, m_post);
            break;
        case TransferType::kPUT:
            ret = setopt(CURLOPT_UPLOAD, 1L);//注意这个是UPLOAD，不是PUT
            break;
    }
    return ret;
}

bool CurlEasyHandleWrapper::setPostContent(
    const std::string& fieldName,
    const std::string& payload
)
{
    CURLFORMcode ret = curl_formadd(
        &m_post,
        &m_lastPost,

        CURLFORM_COPYNAME,
        fieldName.c_str(),

        CURLFORM_COPYCONTENTS,
        payload.c_str(),

        CURLFORM_CONTENTTYPE,
        JSON_MIME_TYPE.c_str(),

        CURLFORM_CONTENTHEADER,
        m_postHeaders,

        CURLFORM_END
    );
    if(ret) {
        mylogd("curl_formadd fail:%d\n", ret);
        return false;
    }
    return true;
}


bool CurlEasyHandleWrapper::setTransferTimeout(const long timeoutSeconds) {
    setopt(CURLOPT_TIMEOUT, timeoutSeconds);
}

bool CurlEasyHandleWrapper::addPostStream(const std::string& fieldName, void *userData) {
    CURLFORMcode ret = curl_formadd(
        &m_post,
        &m_lastPost,

        CURLFORM_COPYNAME,
        fieldName.c_str(),

        CURLFORM_STREAM,
        userData,

        CURLFORM_CONTENTTYPE,
        OCTET_MIME_TYPE.c_str(),

        CURLFORM_END
    );
    if(ret) {
        mylogd("addPostStream fail\n");
        return false;
    }
    return true;
}

bool CurlEasyHandleWrapper::setPostData(const std::string& data) {
    return setopt(CURLOPT_POSTFIELDS, data.c_str());
}

bool CurlEasyHandleWrapper::setConnectionTimeout(const std::chrono::seconds timeoutSeconds) {
    return setopt(CURLOPT_CONNECTTIMEOUT, timeoutSeconds);
}
bool CurlEasyHandleWrapper::setWriteCallback(CurlCallback callback, void *userData) {
    bool ret = false;
    ret = setopt(CURLOPT_WRITEFUNCTION, callback);
    if(ret) {
        return false;
    }
    if(userData) {
        ret = setopt(CURLOPT_WRITEDATA, userData);
    }
    if(ret) {
        return false;
    }
    return true;
}
bool CurlEasyHandleWrapper::setReadCallback(CurlCallback callback, void *userData) {
    bool ret = false;
    ret = setopt(CURLOPT_READFUNCTION, callback);
    if(ret) {
        return false;
    }
    if(userData) {
        ret = setopt(CURLOPT_READDATA, userData);
    }
    if(ret) {
        return false;
    }
    return true;
}
bool CurlEasyHandleWrapper::setHeaderCallback(CurlCallback callback, void *userData) {
    bool ret = false;
    ret = setopt(CURLOPT_HEADERFUNCTION, callback);
    if(ret) {
        return false;
    }
    if(userData) {
        ret = setopt(CURLOPT_HEADERDATA, userData);
    }
    if(ret) {
        return false;
    }
    return true;
}
bool CurlEasyHandleWrapper::setDebugCallback(CurlDebugCallback callback, void *userData) {
    bool ret = false;
    ret = setopt(CURLOPT_VERBOSE, 1L);

    ret = setopt(CURLOPT_DEBUGFUNCTION, callback);
    if(ret) {
        return false;
    }
    if(userData) {
        ret = setopt(CURLOPT_WRITEDATA, userData);
    }
    if(ret) {
        return false;
    }
    return true;
}

std::string CurlEasyHandleWrapper::urlEncode(const std::string& in) const {
    std::string result;
    auto temp = curl_easy_escape(m_handle, in.c_str(), 0);
    if(temp) {
        result = temp;
        curl_free(temp);
    }
    return result;
}

void CurlEasyHandleWrapper::cleanupResources() {
    if(m_requestHeaders) {
        curl_slist_free_all(m_requestHeaders);
        m_requestHeaders = nullptr;
    }
    if(m_postHeaders) {
        curl_slist_free_all(m_postHeaders);
        m_postHeaders = nullptr;
    }
    if(m_post) {
        curl_formfree(m_post);
        m_post = nullptr;
        m_lastPost = nullptr;
    }

}
bool CurlEasyHandleWrapper::setDefaultOptions() {
    if(prepareForTLS(m_handle)) {
        //成功从这里返回。
        //下面这个设置选项是针对多线程的场景需要设置的。
        return setopt(CURLOPT_NOSIGNAL, 1);
    }
    //失败的情况
    mylogd("setDefaultOptions fail\n");
    curl_easy_cleanup(m_handle);
    m_handle = nullptr;
    return false;
}

long CurlEasyHandleWrapper::getHTTPResponseCode() {
    long http_code = 0;
    curl_easy_getinfo(m_handle, CURLINFO_RESPONSE_CODE, &http_code);
    return http_code;
}

CURLcode CurlEasyHandleWrapper::perform() {
    if(!isValid()) {
        return CURLcode::CURLE_FAILED_INIT;
    }
    return curl_easy_perform(m_handle);
}

bool CurlEasyHandleWrapper::reset() {
    //todo

}