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
