#include <gtest/gtest.h>

#include <tinyserver/http/HttpRequest.h>
#include <tinyserver/buffer.h>
#include <tinyserver/logger.h>

using namespace tinyserver;
using namespace tinyserver::http;

TEST(HttpRequest, setMethod)
{
	HttpRequest request;
	request.setMethod("GET");
	EXPECT_EQ(request.method(), HttpRequest::Method::Get);
	EXPECT_EQ(request.methodString(), "GET");
	
	request.setMethod("post");
	EXPECT_EQ(request.method(), HttpRequest::Method::Post);
	EXPECT_EQ(request.methodString(), "POST");

	request.setMethod("Put");
	EXPECT_EQ(request.method(), HttpRequest::Method::Put);
	EXPECT_EQ(request.methodString(), "PUT");

	request.setMethod("Delete");
	EXPECT_EQ(request.method(), HttpRequest::Method::Delete);
	EXPECT_EQ(request.methodString(), "DELETE");

	request.setMethod("");
	EXPECT_EQ(request.method(), HttpRequest::Method::Invalid);
	EXPECT_EQ(request.methodString(), "INVALID");
}

TEST(HttpRequest, setVersion)
{
	HttpRequest request;
	request.setVersion("HTTP/1.0");
	EXPECT_EQ(request.version(), HttpRequest::Version::Http10);
	EXPECT_EQ(request.versionString(), "HTTP/1.0");

	request.setVersion("http/1.1");
	EXPECT_EQ(request.version(), HttpRequest::Version::Http11);
	EXPECT_EQ(request.versionString(), "HTTP/1.1");

	request.setVersion("HTTP/2.0");
	EXPECT_EQ(request.version(), HttpRequest::Version::Unknown);
	EXPECT_EQ(request.versionString(), "UNKNOWN");
}

TEST(HttpRequest, parserRequest)
{
	Buffer buf;
	buf.append("GET / HTTP/1.0\r\n");
	HttpRequest request;
	request.parserRequest(&buf);
	ASSERT_EQ(request.methodString(), "GET");
	ASSERT_EQ(request.path(), "/");
	ASSERT_EQ(request.versionString(), "HTTP/1.0");
	
	buf.retrieveAll();
	buf.append("POST /test HTTP/1.1\r\n");
	request.parserRequest(&buf);
	ASSERT_EQ(request.methodString(), "POST");
	ASSERT_EQ(request.path(), "/test");
	ASSERT_EQ(request.versionString(), "HTTP/1.1");
	
	// 完整的Get请求
	buf.retrieveAll();
	buf.append("GET /test?arg1=&arg2=v2&arg3= HTTP/1.1\r\n"
				"Host: www.tinyserver.com\r\n"
				"Connection: Keep-Alive\r\n"
				"\r\n");
	request.parserRequest(&buf);
	ASSERT_EQ(request.methodString(), "GET");
	ASSERT_EQ(request.path(), "/test");
	ASSERT_EQ(request.versionString(), "HTTP/1.1");
	ASSERT_EQ(request.queries().size(), 3);
	EXPECT_EQ(request.query("arg1"), "");
	EXPECT_EQ(request.query("arg2"), "v2");
	EXPECT_EQ(request.query("arg3"), "");
	EXPECT_EQ(request.query("invalidArg"), "");
	ASSERT_EQ(request.headers().size(), 2);
	EXPECT_EQ(request.header("Host"), "www.tinyserver.com");
	EXPECT_EQ(request.header("Connection"), "Keep-Alive");
	EXPECT_EQ(request.header("invalidHeader"), "");
	
	buf.retrieveAll();
	buf.append("POST /test HTTP/1.1\r\n"
				"Host: www.tinyserver.com\r\n"
				"Content-Type: text/plain;charset=UTF-8\r\n"
				"\r\n"
				"arg1:v1,arg2:v2");
	request.parserRequest(&buf);
	ASSERT_EQ(request.methodString(), "POST");
	EXPECT_EQ(request.header("Content-Type"), "text/plain;charset=UTF-8");
	EXPECT_EQ(request.body(), "arg1:v1,arg2:v2");
}