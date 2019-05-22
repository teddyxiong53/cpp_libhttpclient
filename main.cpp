#include "CurlEasyHandleWrapper.h"
#include "mylog.h"
#include <string>
#include <vector>
#include <memory>
#include <chrono>

/*
//这个是最简单的测试。
int main()
{
	CurlEasyHandleWrapper curl;
	curl.setURL("localhost");
	curl.addHTTPHeader("GET /test.html HTTP/1.1\r\n");
	curl.perform();
	mylogd("return code:%ld\n", curl.getHTTPResponseCode());
}
*/

//这个测试HttpPost。
#include "HTTPResponse.h"
#include "HttpPost.h"
int main()
{
	std::unique_ptr<HttpPost> post = HttpPost::create();
	std::vector<std::pair<std::string, std::string>> data = {
		{"a", "1"},
		{"b", "2"}
	};

	HTTPResponse res = post->doPost("http://192.168.56.101:3000", {}, data, std::chrono::seconds(10));
	mylogd("res:%d", res.code);
}



