#include "CppUTest/TestHarness.h"
extern "C" {
#include "llist.h"
}

TEST_GROUP(llist_test_group)
{
    llist_head_st llist_test_head;

    void
    setup (void)
    {
        llist_init(&llist_test_head);
        LONGS_EQUAL(llist_test_head.list_count, 0);
        POINTERS_EQUAL(llist_test_head.list_head, NULL);
    }

    void
    teardown (void)
    {
        POINTERS_EQUAL(llist_test_head.list_head, NULL);
        LONGS_EQUAL(llist_test_head.list_count, 0);
    }
};

TEST(llist_test_group, InitHead)
{
    llist_test_head.list_head = (struct llist_elem_st_ *)0xdeadbeef;
    llist_test_head.list_count = 10;

    llist_init(&llist_test_head);
    LONGS_EQUAL(llist_test_head.list_count, 0);
    POINTERS_EQUAL(llist_test_head.list_head, NULL);
}

TEST(llist_test_group, AddOneElem)
{
    llist_elem_st elem;

    llist_append(&llist_test_head, &elem);
    POINTERS_EQUAL(llist_test_head.list_head, &elem);
    POINTERS_EQUAL(elem.prev, NULL);
    POINTERS_EQUAL(elem.next, NULL);
    LONGS_EQUAL(llist_test_head.list_count, 1);

    llist_unlink(&llist_test_head, &elem);
    POINTERS_EQUAL(llist_test_head.list_head, NULL);
    POINTERS_EQUAL(elem.prev, NULL);
    POINTERS_EQUAL(elem.next, NULL);
    LONGS_EQUAL(llist_test_head.list_count, 0);
}

TEST(llist_test_group, AddTwoRemoveLast)
{
    llist_elem_st elem[2];

    llist_append(&llist_test_head, &elem[0]);
    llist_append(&llist_test_head, &elem[1]);
    POINTERS_EQUAL(llist_test_head.list_head, &elem[0]);
    POINTERS_EQUAL(elem[0].prev, NULL);
    POINTERS_EQUAL(elem[0].next, &elem[1]);
    POINTERS_EQUAL(elem[1].prev, &elem[0]);
    POINTERS_EQUAL(elem[1].next, NULL);
    LONGS_EQUAL(llist_test_head.list_count, 2);

    llist_unlink(&llist_test_head, &elem[1]);
    LONGS_EQUAL(llist_test_head.list_count, 1);
    POINTERS_EQUAL(llist_test_head.list_head, &elem[0]);
    POINTERS_EQUAL(elem[1].prev, NULL);
    POINTERS_EQUAL(elem[1].next, NULL);
    POINTERS_EQUAL(elem[0].prev, NULL);
    POINTERS_EQUAL(elem[0].next, NULL);

    llist_unlink(&llist_test_head, &elem[0]);
}

TEST(llist_test_group, AddTwoRemoveFirst)
{
    llist_elem_st elem[2];

    llist_append(&llist_test_head, &elem[0]);
    llist_append(&llist_test_head, &elem[1]);
    llist_unlink(&llist_test_head, &elem[0]);
    POINTERS_EQUAL(llist_test_head.list_head, &elem[1]);
    POINTERS_EQUAL(elem[1].prev, NULL);
    POINTERS_EQUAL(elem[1].next, NULL);

    llist_unlink(&llist_test_head, &elem[1]);
}

TEST(llist_test_group, AddThreeRemoveMiddle)
{
    llist_elem_st elem[3];
    int i;

    for (i = 0; i < 3; i++) {
        llist_append(&llist_test_head, &elem[i]);
    }

    POINTERS_EQUAL(llist_test_head.list_head, &elem[0]);

    POINTERS_EQUAL(elem[0].prev, NULL);
    POINTERS_EQUAL(elem[0].next, &elem[1]);

    POINTERS_EQUAL(elem[1].prev, &elem[0]);
    POINTERS_EQUAL(elem[1].next, &elem[2]);

    POINTERS_EQUAL(elem[2].prev, &elem[1]);
    POINTERS_EQUAL(elem[2].next, NULL);

    llist_unlink(&llist_test_head, &elem[1]);
    POINTERS_EQUAL(llist_test_head.list_head, &elem[0]);

    POINTERS_EQUAL(elem[0].next, &elem[2]);
    POINTERS_EQUAL(elem[2].prev, &elem[0]);

    llist_unlink(&llist_test_head, &elem[0]);
    llist_unlink(&llist_test_head, &elem[2]);
}

void
llist_test_walk(llist_elem_st *curr_elem, void *ctx)
{
    int *num_elems_walked = (int *)ctx;
    *num_elems_walked += 1;
}

void
llist_test_destroy_walk(llist_elem_st *curr_elem, void *ctx)
{
    int *num_elems_walked = (int *)ctx;
    *num_elems_walked += 1;
    POINTERS_EQUAL(curr_elem->prev, NULL);
    POINTERS_EQUAL(curr_elem->next, NULL);
}

TEST(llist_test_group, FiveElemWalk)
{
    llist_elem_st elems[5];
    int i = 0;
    int num_elems_walked = 0;

    for (i = 0; i < 5; i++) {
        llist_append(&llist_test_head, &elems[i]);
    }

    llist_walk(&llist_test_head, llist_test_walk,
               (void *)&num_elems_walked);
    LONGS_EQUAL(llist_test_head.list_count, num_elems_walked);

    num_elems_walked = 0;

    llist_walk_destroy(&llist_test_head, llist_test_destroy_walk,
                       (void *)&num_elems_walked);
}
