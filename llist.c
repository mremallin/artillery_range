#include "llist.h"

#include <assert.h>
#include <string.h>

void
llist_init (llist_head_st *head)
{
    memset(head, 0, sizeof(*head));
}

void
llist_append (llist_head_st *head,
               llist_elem_st *elem)
{
    llist_elem_st *current = NULL;

    assert(head);

    if (head->list_head == NULL) {
        head->list_head = elem;
        elem->next = NULL;
        elem->prev = NULL;

        return;
    }

    current = head->list_head;
    while (current && current->next != NULL) {
        current = current->next;
    }

    current->next = elem;
    elem->next = NULL;
    elem->prev = current;
}

void
llist_walk (llist_head_st *head,
            llist_walk_func walker,
            void *ctx)
{
    llist_elem_st *elem = NULL;

    assert(head);

    elem = head->list_head;
    while (elem) {
        walker(elem, ctx);
        elem = elem->next;
    }
}

void
llist_unlink (llist_head_st *head,
              llist_elem_st *elem)
{
    llist_elem_st *prev_elem, *next_elem;

    prev_elem = elem->prev;
    next_elem = elem->next;

    if (prev_elem) {
        prev_elem->next = next_elem;
    } else {
        /* No previous means we're the first element. Unlink
         * from the head. */
        head->list_head = NULL;
    }

    if (next_elem) {
        next_elem->prev = prev_elem;
    }

    elem->next = NULL;
    elem->prev = NULL;
}

void
llist_walk_destroy(llist_head_st *head,
                   llist_walk_func walker,
                   void *ctx)
{
    llist_elem_st *elem = NULL;
    llist_elem_st *old_elem = NULL;

    assert(head);

    elem = head->list_head;
    while (elem) {
        old_elem = elem;
        elem = elem->next;
        llist_unlink(head, old_elem);
        walker(old_elem, ctx);
    }
}
