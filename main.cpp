#include "CurlEasyHandleWrapper.h"

int main()
{
	CurlEasyHandleWrapper curl;
	curl.setURL("localhost");
	curl.addHTTPHeader("GET / HTTP/1.1\r\n");
	curl.perform();
	printf("return code:%ld\n", curl.getHTTPResponseCode());
}



