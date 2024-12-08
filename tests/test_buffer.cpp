#include <gtest/gtest.h>

#include "cc/buffer.hxx"

TEST(BufferTest, Basic)
{
        cc::buffer<float, 8> data;
        ASSERT_EQ(data.size(), 0);
        ASSERT_EQ(data.max_size(), 8);

        int count = 0;
        for (auto& v : data)
        {
                count++;
        }
        ASSERT_EQ(count, 0);

        data.assign(0, 1.0f);
        data.assign(1, 2.0f);

        ASSERT_EQ(data.size(), 2);

        count = 0;
        for (auto& v : data)
        {
                count++;
        }
        ASSERT_EQ(count, 2);

        data.push_back(3.0f);
        ASSERT_EQ(data.size(), 3);

        ASSERT_EQ(data.array().size(), 8);

        ASSERT_EQ(data[0], 1.0f);
        ASSERT_EQ(data[1], 2.0f);
        ASSERT_EQ(data[2], 3.0f);
        ASSERT_EQ(data.array()[0], 1.0f);
        ASSERT_EQ(data.array()[1], 2.0f);
        ASSERT_EQ(data.array()[2], 3.0f);
}
