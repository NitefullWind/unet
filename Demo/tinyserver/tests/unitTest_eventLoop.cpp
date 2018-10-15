#include <gtest/gtest.h>

#include <tinyserver/eventLoop.h>
#include <tinyserver/logger.h>

TEST(TestEventLoop, Test1)
{
	EXPECT_EQ(1, 1);
}

TEST(TestEventLoop, loop)
{
	tinyserver::EventLoop loop;
	EXPECT_TRUE(loop.loop());
}
