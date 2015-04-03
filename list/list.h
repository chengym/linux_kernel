#ifndef _LIST_H
#define _LIST_H

typedef _list_head {
    struct _list_head *prev;
    struct _list_head *next;
} list_head;

#define LIST_HEAD_INIT(name) { &(name), &(name) }

#define LIST_HEAD(name) \
	list_head name = LIST_HEAD_INIT(name)

#define offsetof(type, member) (size_t)&(((type*)0)->member)

#define container_of(ptr, type, member) ({                  \
        const typeof(((type *)0)->member) *__mptr = (ptr);  \
        (type*)((char*)__mptr - offsetof(type,member));})

static inline void INIT_LIST_HEAD(list_head *list)
{
	list->next = list;
	list->prev = list;
}

static inline void __list_add(list_head *new,
			      list_head *prev,
			      list_head *next)
{
	next->prev = new;
	new->next = next;
	new->prev = prev;
	prev->next = new;
}

static inline void list_add(list_head *new, list_head *head)
{
	__list_add(new, head, head->next);
}

static inline void list_add_tail(list_head *new, list_head *head)
{
	__list_add(new, head->prev, head);
}

static inline void __list_del(list_head * prev, list_head * next)
{
	next->prev = prev;
	prev->next = next;
}

static inline void __list_del_entry(list_head *entry)
{
	__list_del(entry->prev, entry->next);
}

static inline void list_del(list_head *entry)
{
	__list_del(entry->prev, entry->next);
	entry->next = LIST_POISON1;
	entry->prev = LIST_POISON2;
}

static inline void list_replace(list_head *old,
				list_head *new)
{
	new->next = old->next;
	new->next->prev = new;
	new->prev = old->prev;
	new->prev->next = new;
}

static inline void list_replace_init(list_head *old,
					list_head *new)
{
	list_replace(old, new);
	INIT_LIST_HEAD(old);
}

static inline void list_del_init(list_head *entry)
{
	__list_del_entry(entry);
	INIT_LIST_HEAD(entry);
}

static inline void list_move(list_head *list, list_head *head)
{
	__list_del_entry(list);
	list_add(list, head);
}

static inline void list_move_tail(list_head *list,
				  list_head *head)
{
	__list_del_entry(list);
	list_add_tail(list, head);
}

static inline int list_is_last(const list_head *list,
				const list_head *head)
{
	return list->next == head;
}

static inline int list_empty(const list_head *head)
{
	return head->next == head;
}

/* 
 * get type pointer according to member pointer 
 *
 *  ptr: type list_head
 *  type: entry
 *  member: the member in struct type
 */
#define list_entry(ptr, type, member) \
    container_of(ptr, type, member)
#define list_first_entry(ptr, type, member) \
    list_entry((ptr)->next, type, member)
#define list_last_entry(ptr, type, member) \
	list_entry((ptr)->prev, type, member)
#define list_first_entry_or_null(ptr, type, member) \
	(!list_empty(ptr) ? list_first_entry(ptr, type, member) : NULL)

/* 
 * get the next entry pinter according to the prev entry pointer
 *
 *  ptr: type entry
 *  member: the member in struct entry
 */
#define list_next_entry(pos, member) \
	list_entry((pos)->member.next, typeof(*(pos)), member)
#define list_prev_entry(pos, member) \
	list_entry((pos)->member.prev, typeof(*(pos)), member)

/*
 * iterate over a list by list_head
 *
 * pos: a temp pointer which point list_head, who is emmbed in entry
 * head: the list head pointer
 *
 */
#define list_for_each(pos, head) \
	for (pos = (head)->next; pos != (head); pos = pos->next)
#define list_for_each_prev(pos, head) \
	for (pos = (head)->prev; pos != (head); pos = pos->prev)
#define list_for_each_safe(pos, n, head) \
	for (pos = (head)->next, n = pos->next; pos != (head); \
		pos = n, n = pos->next)
#define list_for_each_prev_safe(pos, n, head)   \
	for (pos = (head)->prev, n = pos->prev;     \
	     pos != (head);                         \
	     pos = n, n = pos->prev)

/*
 * iterate over list by entry
 * pos:	a temp poniter which point (entry) struct
 * head: the head for your list.
 * member: the name of the list_head within the (entry) struct.
 */
#define list_for_each_entry(pos, head, member)                  \
	for (pos = list_first_entry(head, typeof(*pos), member);    \
	     prefetch(pos->member.next), &pos->member != (head);    \
	     pos = list_next_entry(pos, member))
#define list_for_each_entry_reverse(pos, head, member)          \
	for (pos = list_last_entry(head, typeof(*pos), member);	    \
	     prefetch(pos->member.prev), &pos->member != (head);    \
	     pos = list_prev_entry(pos, member))

/*
 * iterate a list start from specified entry
 * pos:	temp point which point (entry) struct
 * head: the head of the list
 * member: the name of the list_head within the (entry) struct.
 */
#define list_prepare_entry(pos, head, member) \
	((pos) ? : list_entry(head, typeof(*pos), member))
#define list_for_each_entry_continue(pos, head, member)         \
	for (pos = list_next_entry(pos, member);                    \
	     prefetch(pos->member.next), &pos->member != (head);    \
	     pos = list_next_entry(pos, member))
#define list_for_each_entry_continue_reverse(pos, head, member)	    \
	for (pos = list_prev_entry(pos, member);                        \
	     &pos->member != (head);                                    \
	     pos = list_prev_entry(pos, member))

/*
 * iterate a list start from current entry
 * pos:	the type * to use as a loop cursor.
 * head:	the head for your list.
 * member:	the name of the list_head within the struct.
 */
#define list_for_each_entry_from(pos, head, member) \
	for (; &pos->member != (head);                  \
	     pos = list_next_entry(pos, member))

#endif /* end _LIST_H_ */
