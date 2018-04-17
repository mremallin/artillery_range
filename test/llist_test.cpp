#include "CppUTest/TestHarness.h"
extern "C" {
#include "llist.h"
}

TEST_GROUP(llist_test_group)
{
};

TEST(llist_test_group, InitNullHead)
{
   llist_init(NULL);
}

TEST(llist_test_group, InitHead)
{
    llist_head_st test_head;

    test_head.list_head = (struct llist_elem_st_ *)0xdeadbeef;
    test_head.list_count = 10;

    llist_init(&test_head);
    LONGS_EQUAL(test_head.list_count, 0);
    POINTERS_EQUAL(test_head.list_head, NULL);
}
