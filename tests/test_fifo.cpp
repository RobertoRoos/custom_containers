#include <gtest/gtest.h>

#include <vector>

#include "cc/fifo.hxx"

TEST(FifoTest, Empty)
{
	cc::fifo<float, 10> data;

	ASSERT_EQ(data.max_size(), 10);
	ASSERT_EQ(data.size(), 0);
	ASSERT_TRUE(data.empty());
	ASSERT_EQ(data.free(), 10);
}

TEST(FifoTest, PushFull)
{
	cc::fifo<float, 3> data;
	data.push(1.0f);
	data.push(2.0f);
	data.push(3.0f);

	ASSERT_EQ(data.size(), 3);
	ASSERT_EQ(data.free(), 0);
	ASSERT_TRUE(data.full());

	ASSERT_THROW({ data.push(4.0f); }, std::out_of_range);
}

TEST(FifoTest, PushPop)
{
	cc::fifo<float, 5> data;

	data.push(1.0f);
	data.push(2.0f);
	ASSERT_EQ(data.size(), 2);

	ASSERT_EQ(data.pop(), 1.0f);
	ASSERT_EQ(data.size(), 1);

	ASSERT_EQ(data.pop(), 2.0f);
	ASSERT_EQ(data.size(), 0);

	data.push(3.0f);
	data.push(4.0f);
	data.push(5.0f);
	data.push(6.0f);
	data.push(7.0f);

	ASSERT_EQ(data.size(), 5);
	ASSERT_EQ(data.free(), 0);
	ASSERT_TRUE(data.full());

	ASSERT_EQ(data.pop(), 3.0f);
	ASSERT_EQ(data.pop(), 4.0f);
	ASSERT_EQ(data.pop(), 5.0f);
	ASSERT_EQ(data.pop(), 6.0f);
	ASSERT_EQ(data.pop(), 7.0f);

	ASSERT_EQ(data.size(), 0);
	ASSERT_EQ(data.free(), 5);
	ASSERT_TRUE(data.empty());
}

TEST(FifoTest, PushList)
{
	cc::fifo<float, 5> data;

	std::array<float, 3> src{1.0f, 2.0f, 3.0f};

	data.push_list(src.begin(), src.end());

	ASSERT_EQ(data.size(), 3);
	ASSERT_EQ(data.pop(), 1.0f);
	ASSERT_EQ(data.pop(), 2.0f);
	ASSERT_EQ(data.pop(), 3.0f);
	ASSERT_TRUE(data.empty());

	data.push_list(src.begin(), src.end());

	ASSERT_EQ(data.size(), 3);
	ASSERT_EQ(data.pop(), 1.0f);
	ASSERT_EQ(data.pop(), 2.0f);
	ASSERT_EQ(data.pop(), 3.0f);
	ASSERT_TRUE(data.empty());
}

TEST(FifoTest, PopList)
{
	cc::fifo<float, 5> data;

	data.push(1.0f);
	data.push(2.0f);
	data.push(3.0f);
	ASSERT_EQ(data.size(), 3);

	std::array<float, 3> dst{};
	data.pop_list(dst.begin());
	ASSERT_EQ(dst[0], 1.0f);
	ASSERT_EQ(dst[1], 2.0f);
	ASSERT_EQ(dst[2], 3.0f);
	ASSERT_EQ(data.size(), 0);

	data.push(1.0f);
	data.push(2.0f);
	data.push(3.0f);
	ASSERT_EQ(data.size(), 3);

	dst.fill(0.0f);
	data.pop_list(dst.begin(), 3);
	ASSERT_EQ(dst[0], 1.0f);
	ASSERT_EQ(dst[1], 2.0f);
	ASSERT_EQ(dst[2], 3.0f);
	ASSERT_EQ(data.size(), 0);
}


TEST(FifoTest, ForLoop)
{
	cc::fifo<float, 5> data;
	data.push(1.0f);
	data.push(2.0f);
	data.push(3.0f);

	float check = 1.0f;
	for(const auto& v : data) {
		ASSERT_EQ(check, v);
		check += 1.0f;
	}
}
