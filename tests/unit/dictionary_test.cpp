#include <gtest/gtest.h>

#include "dictionary.h"


TEST(Dictionary, get_onMissingValue_fails)
{
    Dictionary dict{false};

    EXPECT_FALSE(dict.get("abc"));
}

TEST(Dictionary, get_onExistingValue_succeds)
{
    Dictionary dict{false};

    dict.set("abc", "123");

    auto valFromDict = dict.get("abc");
    EXPECT_EQ(valFromDict, "123");
}

TEST(Dictionary, set_multipleValues_succeds)
{
    Dictionary dict{false};

    dict.set("abc", "123");
    dict.set("def", "456");

    EXPECT_TRUE(dict.get("abc"));
    EXPECT_TRUE(dict.get("def"));
    EXPECT_FALSE(dict.get("ghi"));
}

TEST(Dictionary, get_withBloomFilter_succeeds)
{
    Dictionary dict{true};

    dict.set("abc", "123");
    dict.set("def", "456");

    EXPECT_TRUE(dict.get("abc"));
    EXPECT_TRUE(dict.get("def"));
    EXPECT_FALSE(dict.get("ghi"));
}
