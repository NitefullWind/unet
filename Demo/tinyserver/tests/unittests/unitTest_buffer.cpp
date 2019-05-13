#include <gtest/gtest.h>

#include <tinyserver/buffer.h>
#include <tinyserver/logger.h>

using namespace tinyserver;


TEST(Buffer, append)
{
	std::string str = "abc";
	Buffer buf;
	buf.append(str);
	ASSERT_EQ(buf.readableBytes(), str.length());
	ASSERT_EQ(buf.readAll(), str);
	ASSERT_EQ(buf.readableBytes(), 0);
}

TEST(Buffer, retrieve)
{
	Buffer buf;
	buf.append("abc");
	buf.retrieve(1);
	EXPECT_EQ(buf.readAll(), "bc");

	buf.append("abc");
	buf.retrieveAll();
	EXPECT_EQ(buf.readAll(), "");
}

TEST(Buffer, findCRLF)
{
	const char crlf[2] = {'\r', '\n'};
	
	Buffer buf;
	buf.append("abc");
	EXPECT_EQ(buf.findCRLF(), nullptr);

	buf.retrieveAll();
	buf.append("\n");
	EXPECT_TRUE(memcmp(buf.findCRLF(), crlf+1, 1) == 0);

	buf.retrieveAll();
	buf.append("abc\r\n");
	EXPECT_TRUE(memcmp(buf.findCRLF(), crlf, 2) == 0);
}

TEST(Buffer, findEOL)
{	
	Buffer buf;
	buf.append("abc");
	EXPECT_EQ(buf.findEOL(), nullptr);

	buf.retrieveAll();
	buf.append("\n");
	ASSERT_TRUE(memcmp(buf.findEOL(), "\n", 1) == 0);

	buf.retrieveAll();
	buf.append("abc\r\n");
	ASSERT_TRUE(memcmp(buf.findEOL(), "\n", 1) == 0);
}

TEST(Buffer, readLine)
{
	std::string str = "";
	Buffer buf;
	buf.append(str);
	EXPECT_EQ(buf.readLine(), str);

	buf.retrieveAll();
	str = "abc";
	buf.append(str);
	EXPECT_EQ(buf.readLine(), str);

	buf.retrieveAll();
	str = "\n";
	buf.append(str);
	EXPECT_EQ(buf.readLine(), str);

	buf.retrieveAll();
	str = "\n";
	buf.append(str);
	EXPECT_EQ(buf.readLine(false), "");

	buf.retrieveAll();
	str = "abc\n";
	buf.append(str);
	EXPECT_EQ(buf.readLine(false), "abc");
	
	buf.retrieveAll();
	str = "abc\n";
	buf.append(str);
	buf.append("def");
	EXPECT_EQ(buf.readLine(), str);
	EXPECT_EQ(buf.readLine(), "def");

	buf.retrieveAll();
	str = "abc\r\n";
	buf.append(str);
	buf.append("def");
	EXPECT_EQ(buf.readLine(false), "abc");
	EXPECT_EQ(buf.readLine(), "def");
}