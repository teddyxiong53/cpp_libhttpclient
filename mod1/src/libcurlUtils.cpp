#include <cerrno>
#include <cstdlib>

#include "libcurlUtils.h"
#include "mylog.h"
#define SETOPT(handle, option, value)  setopt(handle, option, value, #option, #value)

template <typename ValueType>
static bool setopt(
    CURL* handle,
    CURLoption option,
    ValueType value,
    const char *optionName,
    const char *valueAsString
)
{
    auto result = curl_easy_setopt(handle, option, value);
    if(result != CURLE_OK) {
        myloge("curl_easy_setopt fail: %d\n", result);
        return false;
    }
    return true;
}

bool prepareForTLS(CURL* handle) {
    if(!handle) {
        myloge("handle is null\n");
        return false;
    }
    int ret = SETOPT(handle, CURLOPT_SSLVERSION, CURL_SSLVERSION_TLSv1_2);
    mylogd("ret:%d", ret);
    #if 0
    if(!(SETOPT(handle, CURLOPT_SSLVERSION, CURL_SSLVERSION_TLSv1_2)
        && SETOPT(handle, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_2_0)
        && SETOPT(handle, CURLOPT_USE_SSL, CURLUSESSL_ALL)
        && SETOPT(handle, CURLOPT_SSL_VERIFYHOST, 2L)
        ))
    {
        return false;
    }
    #endif
    return true;

}
