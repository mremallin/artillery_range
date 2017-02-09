#ifndef __LLIST_H__
#define __LLIST_H__

#include <stdint.h>

typedef struct llist_elem_st_ {
    struct llist_elem_st_ *next;
    struct llist_elem_st_ *prev;
} llist_elem_st;

typedef struct llist_head_st_ {
    struct llist_elem_st_ *list_head;
    uint32_t list_count;
} llist_head_st;

typedef void (*llist_walk_func)(llist_elem_st *, void *);

void
llist_init(llist_head_st *);

void
llist_append(llist_head_st *,
             llist_elem_st *);

void
llist_walk(llist_head_st *head,
           llist_walk_func walker,
           void *ctx);

void
llist_unlink(llist_elem_st *elem);

void
llist_walk_destroy(llist_head_st *head,
                   llist_walk_func walker,
                   void *ctx);

#endif /* __LLIST_H__ */
