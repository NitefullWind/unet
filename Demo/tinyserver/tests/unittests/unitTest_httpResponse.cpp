#include <gtest/gtest.h>

#include <tinyserver/buffer.h>
#include <tinyserver/logger.h>
#include <tinyserver/http/HttpResponse.h>

using namespace tinyserver;
using namespace tinyserver::http;

TEST(HttpResponse, copy)
{
	HttpResponse rsp;
	rsp.setStatusCode(200);
	rsp.setStatusMessage("ok");
	rsp.setHeader("header", "test");
	rsp.setTextBody("hi");

	HttpResponse copyRsp1(rsp);
	EXPECT_EQ(copyRsp1.statusCode(), rsp.statusCode());
	EXPECT_EQ(copyRsp1.statusMessage(), rsp.statusMessage());
	EXPECT_EQ(copyRsp1.headers().size(), rsp.headers().size());
	for (auto &&v : rsp.headers()) {
		EXPECT_EQ(v.second, copyRsp1.header(v.first));
	}
	EXPECT_EQ(copyRsp1.textBody(), rsp.textBody());

	HttpResponse copyRsp2 = rsp;
	EXPECT_EQ(copyRsp2.statusCode(), rsp.statusCode());
	EXPECT_EQ(copyRsp2.statusMessage(), rsp.statusMessage());
	EXPECT_EQ(copyRsp2.headers().size(), rsp.headers().size());
	for (auto &&v : rsp.headers()) {
		EXPECT_EQ(v.second, copyRsp2.header(v.first));
	}
	EXPECT_EQ(copyRsp2.textBody(), rsp.textBody());
}

TEST(HttpResponse, move)
{
	HttpResponse rsp, copyRsp;
	rsp.setStatusCode(200);
	rsp.setStatusMessage("ok");
	rsp.setHeader("header", "test");
	rsp.setTextBody("hi");
	copyRsp = rsp;	// copy for check

	HttpResponse moveRsp1(std::move(rsp));
	EXPECT_EQ(rsp.statusMessage(), "");
	EXPECT_EQ(rsp.headers().size(), 0);
	EXPECT_EQ(rsp.textBody(), "");
	EXPECT_EQ(moveRsp1.statusCode(), copyRsp.statusCode());
	EXPECT_EQ(moveRsp1.statusMessage(), copyRsp.statusMessage());
	EXPECT_EQ(moveRsp1.headers().size(), copyRsp.headers().size());
	for (auto &&v : copyRsp.headers()) {
		EXPECT_EQ(v.second, moveRsp1.header(v.first));
	}
	EXPECT_EQ(moveRsp1.textBody(), copyRsp.textBody());

	rsp = copyRsp;	// copy for check
	HttpResponse copyRsp2 = std::move(rsp);
	EXPECT_EQ(rsp.statusMessage(), "");
	EXPECT_EQ(rsp.headers().size(), 0);
	EXPECT_EQ(rsp.textBody(), "");
	EXPECT_EQ(copyRsp2.statusCode(), copyRsp.statusCode());
	EXPECT_EQ(copyRsp2.statusMessage(), copyRsp.statusMessage());
	EXPECT_EQ(copyRsp2.headers().size(), copyRsp.headers().size());
	for (auto &&v : copyRsp.headers()) {
		EXPECT_EQ(v.second, copyRsp2.header(v.first));
	}
	EXPECT_EQ(copyRsp2.textBody(), copyRsp.textBody());
}

TEST(HttpResponse, getResponseBuffer)
{
	HttpResponse rsp;
	rsp.setStatusCode(200);
	rsp.setStatusMessage("ok");
	rsp.setContentType("text/html");
	rsp.setKeepAlive(false);
	rsp.setHeader("header", "test");
	rsp.setTextBody("hi");

	Buffer buffer;
	rsp.getResponseBuffer(&buffer);

	std::string rspText = "HTTP/1.1 200 ok\r\nConnection: close\r\nContent-Length: 2\r\nContent-Type: text/html\r\nheader: test\r\n\r\nhi";
	EXPECT_EQ(buffer.readAll(), rspText);
}