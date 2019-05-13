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
	buf.append("GET / HTTP/1.0\n");
	HttpRequest request;
	request.parserRequest(&buf);
	EXPECT_EQ(request.methodString(), "GET");
	EXPECT_EQ(request.path(), "/");
	EXPECT_EQ(request.versionString(), "HTTP/1.0");
	
	buf.retrieveAll();
	buf.append("POST /test HTTP/1.1\n");
	request.parserRequest(&buf);
	EXPECT_EQ(request.methodString(), "POST");
	EXPECT_EQ(request.path(), "/test");
	EXPECT_EQ(request.versionString(), "HTTP/1.1");
}