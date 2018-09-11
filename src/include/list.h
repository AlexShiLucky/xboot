#ifndef __LIST_H__
#define __LIST_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <types.h>
#include <stddef.h>

/*
 * Simple doubly linked list implementation.
 *
 * Some of the internal functions ("__xxx") are useful when
 * manipulating whole lists rather than single entries, as
 * sometimes we already know the next/prev entries and we can
 * generate better code by using them directly rather than
 * using the generic single-entry routines.
 */
/* 双向链表结构 */
struct list_head {
	struct list_head * next, * prev;
};

/* 定义双向链表头并初始化宏 */
#define LIST_HEAD(name) \
	struct list_head name = { &(name), &(name) }

static inline void init_list_head(struct list_head * list)
{
	list->next = list;
	list->prev = list;
}

/*
 * Insert a new entry between two known consecutive entries.
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
/* 在prev和next节点之间添加new节点 */
static inline void __list_add(struct list_head * new,
			      struct list_head * prev,
			      struct list_head * next)
{
	next->prev = new;
	new->next = next;
	new->prev = prev;
	prev->next = new;
}

/**
 * list_add - add a new entry
 * @new: new entry to be added
 * @head: list head to add it after
 *
 * Insert a new entry after the specified head.
 * This is good for implementing stacks.
 */
/* 在head节点后添加一个new节点 */
static inline void list_add(struct list_head * new, struct list_head * head)
{
	__list_add(new, head, head->next);
}

/**
 * list_add_tail - add a new entry
 * @new: new entry to be added
 * @head: list head to add it before
 *
 * Insert a new entry before the specified head.
 * This is useful for implementing queues.
 */
/* 在head节点前添加一个new节点 */
static inline void list_add_tail(struct list_head * new, struct list_head * head)
{
	__list_add(new, head->prev, head);
}

/*
 * Delete a list entry by making the prev/next entries
 * point to each other.
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
/* 删除prev和next节点之间的节点 */
static inline void __list_del(struct list_head * prev, struct list_head * next)
{
	next->prev = prev;
	prev->next = next;
}

/**
 * list_del - deletes entry from list.
 * @entry: the element to delete from the list.
 * Note: list_empty() on entry does not return true after this, the entry is
 * in an undefined state.
 */
/* 删除一个节点 */
static inline void __list_del_entry(struct list_head * entry)
{
	__list_del(entry->prev, entry->next);
}

static inline void list_del(struct list_head *entry)
{
	__list_del_entry(entry);
	entry->next = 0;
	entry->prev = 0;
}

/**
 * list_replace - replace old entry by new one
 * @old : the element to be replaced
 * @new : the new element to insert
 *
 * If @old was empty, it will be overwritten.
 */
/* 将一个old节点替换为new节点 */
static inline void list_replace(struct list_head * old,
				struct list_head * new)
{
	new->next = old->next;
	new->next->prev = new;
	new->prev = old->prev;
	new->prev->next = new;
}

static inline void list_replace_init(struct list_head * old,
					struct list_head * new)
{
	list_replace(old, new);
	init_list_head(old);
}

/**
 * list_del_init - deletes entry from list and reinitialize it.
 * @entry: the element to delete from the list.
 */
static inline void list_del_init(struct list_head * entry)
{
	__list_del_entry(entry);
	init_list_head(entry);
}

/**
 * list_move - delete from one list and add as another's head
 * @list: the entry to move
 * @head: the head that will precede our entry
 */
/* 将list节点移到head节点后 */
static inline void list_move(struct list_head * list, struct list_head * head)
{
	__list_del_entry(list);
	list_add(list, head);
}

/**
 * list_move_tail - delete from one list and add as another's tail
 * @list: the entry to move
 * @head: the head that will follow our entry
 */
/* 将list节点移到head节点前 */
static inline void list_move_tail(struct list_head * list,
				  struct list_head * head)
{
	__list_del_entry(list);
	list_add_tail(list, head);
}

/**
 * list_is_last - tests whether @list is the last entry in list @head
 * @list: the entry to test
 * @head: the head of the list
 */
/* 测试list节点是否为最后一个节点 */
static inline int list_is_last(const struct list_head * list,
				const struct list_head * head)
{
	return list->next == head;
}

/**
 * list_empty - tests whether a list is empty
 * @head: the list to test.
 */
/* 测试链表是否为空链表 */
static inline int list_empty(const struct list_head * head)
{
	return head->next == head;
}

/**
 * list_empty_careful - tests whether a list is empty and not being modified
 * @head: the list to test
 *
 * Description:
 * tests whether a list is empty _and_ checks that no other CPU might be
 * in the process of modifying either member (next or prev)
 *
 * NOTE: using list_empty_careful() without synchronization
 * can only be safe if the only activity that can happen
 * to the list entry is list_del_init(). Eg. it cannot be used
 * if another CPU could re-list_add() it.
 */
/* 仔细测试链表是否为空链表 */
static inline int list_empty_careful(const struct list_head * head)
{
	struct list_head * next = head->next;
	return (next == head) && (next == head->prev);
}

/**
 * list_rotate_left - rotate the list to the left
 * @head: the head of the list
 */
/* 链表左旋 */
static inline void list_rotate_left(struct list_head * head)
{
	struct list_head * first;

	if (!list_empty(head))
	{
		first = head->next;
		list_move_tail(first, head);
	}
}

/**
 * list_is_singular - tests whether a list has just one entry.
 * @head: the list to test.
 */
/* 测试链表是否为单节点链表 */
static inline int list_is_singular(const struct list_head * head)
{
	return !list_empty(head) && (head->next == head->prev);
}

static inline void __list_cut_position(struct list_head * list,
		struct list_head * head, struct list_head * entry)
{
	struct list_head * new_first = entry->next;
	list->next = head->next;
	list->next->prev = list;
	list->prev = entry;
	entry->next = list;
	head->next = new_first;
	new_first->prev = head;
}

/**
 * list_cut_position - cut a list into two
 * @list: a new list to add all removed entries
 * @head: a list with entries
 * @entry: an entry within head, could be the head itself
 *	and if so we won't cut the list
 *
 * This helper moves the initial part of @head, up to and
 * including @entry, from @head to @list. You should
 * pass on @entry an element you know is on @head. @list
 * should be an empty list or a list you do not care about
 * losing its data.
 *
 */
static inline void list_cut_position(struct list_head * list,
		struct list_head * head, struct list_head * entry)
{
	if(list_empty(head))
		return;
	if(list_is_singular(head) &&
		(head->next != entry && head != entry))
		return;
	if(entry == head)
		init_list_head(list);
	else
		__list_cut_position(list, head, entry);
}

static inline void __list_splice(const struct list_head * list,
				 struct list_head * prev,
				 struct list_head * next)
{
	struct list_head * first = list->next;
	struct list_head * last = list->prev;

	first->prev = prev;
	prev->next = first;

	last->next = next;
	next->prev = last;
}

/**
 * list_splice - join two lists, this is designed for stacks
 * @list: the new list to add.
 * @head: the place to add it in the first list.
 */
static inline void list_splice(const struct list_head * list,
				struct list_head * head)
{
	if(!list_empty(list))
		__list_splice(list, head, head->next);
}

/**
 * list_splice_tail - join two lists, each list being a queue
 * @list: the new list to add.
 * @head: the place to add it in the first list.
 */
static inline void list_splice_tail(struct list_head * list,
				struct list_head * head)
{
	if(!list_empty(list))
		__list_splice(list, head->prev, head);
}

/**
 * list_splice_init - join two lists and reinitialise the emptied list.
 * @list: the new list to add.
 * @head: the place to add it in the first list.
 *
 * The list at @list is reinitialised
 */
static inline void list_splice_init(struct list_head * list,
				    struct list_head * head)
{
	if(!list_empty(list))
	{
		__list_splice(list, head, head->next);
		init_list_head(list);
	}
}

/**
 * list_splice_tail_init - join two lists and reinitialise the emptied list
 * @list: the new list to add.
 * @head: the place to add it in the first list.
 *
 * Each of the lists is a queue.
 * The list at @list is reinitialised
 */
static inline void list_splice_tail_init(struct list_head * list,
					 struct list_head * head)
{
	if(!list_empty(list))
	{
		__list_splice(list, head->prev, head);
		init_list_head(list);
	}
}

/**
 * container_of - cast a member of a structure out to the containing structure
 * @ptr:        the pointer to the member.(结构体成员指针)
 * @type:       the type of the container struct this is embedded in.(结构体类型)
 * @member:     the name of the member within the struct.(结构体中成员)
 *
 */
/* 根据结构体成员地址获得结构地址 */
#define container_of(ptr, type, member) ({ \
        const typeof( ((type *)0)->member ) *__mptr = (ptr); \
        (type *)( (char *)__mptr - offsetof(type,member) );})

/**
 * list_entry - get the struct for this entry
 * @ptr:	the &struct list_head pointer.(结构体中链表成员指针)
 * @type:	the type of the struct this is embedded in.(结构体类型)
 * @member:	the name of the list_head within the struct.(结构体中链表成员)
 */
/* 根据当前链表成员指针计算当前节点结构地址 */
#define list_entry(ptr, type, member) \
	container_of(ptr, type, member)

/**
 * list_first_entry - get the first element from a list
 * @ptr:	the list head to take the element from.(表头指针)
 * @type:	the type of the struct this is embedded in.(结构体类型)
 * @member:	the name of the list_head within the struct.(结构体中的链表成员)
 *
 * Note, that list is expected to be not empty.
 */
/* 根据链表表头指针计算第一个节点结构体地址 */
#define list_first_entry(ptr, type, member) \
	list_entry((ptr)->next, type, member)

/**
 * list_last_entry - get the last element from a list
 * @ptr:	the list head to take the element from.(表头指针)
 * @type:	the type of the struct this is embedded in.(结构体类型)
 * @member:	the name of the list_head within the struct.(结构体中的链表成员)
 *
 * Note, that list is expected to be not empty.
 */
/* 根据链表表头指针计算最后一个节点结构体地址 */
#define list_last_entry(ptr, type, member) \
	list_entry((ptr)->prev, type, member)

/**
 * list_first_entry_or_null - get the first element from a list
 * @ptr:	the list head to take the element from.
 * @type:	the type of the struct this is embedded in.
 * @member:	the name of the list_head within the struct.
 *
 * Note that if the list is empty, it returns NULL.
 */
#define list_first_entry_or_null(ptr, type, member) \
	(!list_empty(ptr) ? list_first_entry(ptr, type, member) : NULL)

/**
 * list_next_entry - get the next element in list
 * @pos:	the type * to cursor(当前结构体指针)
 * @member:	the name of the list_head within the struct.(结构体中链表成员)
 */
/* 根据当前结构体节点指针计算下一个结构体节点指针 */
#define list_next_entry(pos, member) \
	list_entry((pos)->member.next, typeof(*(pos)), member)

/**
 * list_prev_entry - get the prev element in list
 * @pos:	the type * to cursor(当前结构体指针)
 * @member:	the name of the list_head within the struct.(结构体中链表成员)
 */
/* 根据当前结构体节点指针计算前一个结构体节点指针 */
#define list_prev_entry(pos, member) \
	list_entry((pos)->member.prev, typeof(*(pos)), member)

/**
 * list_for_each	-	iterate over a list
 * @pos:	the &struct list_head to use as a loop cursor.(当前链表指针)
 * @head:	the head for your list.(链表表头)
 */
/* 往后遍历所有链表节点(在循环中不可用list_del(pos),否则导致死循环) */
#define list_for_each(pos, head) \
	for (pos = (head)->next; pos != (head); pos = pos->next)

/**
 * list_for_each_prev	-	iterate over a list backwards
 * @pos:	the &struct list_head to use as a loop cursor.(当前链表指针)
 * @head:	the head for your list.(链表表头)
 */
/* 往前遍历所有链表节点(在循环中不可用list_del(pos),否则导致死循环) */
#define list_for_each_prev(pos, head) \
	for (pos = (head)->prev; pos != (head); pos = pos->prev)

/**
 * list_for_each_safe - iterate over a list safe against removal of list entry
 * @pos:	the &struct list_head to use as a loop cursor.(当前链表指针)
 * @n:		another &struct list_head to use as temporary storage(下一个链表指针)
 * @head:	the head for your list.(链表表头)
 */
/* 往后遍历所有链表节点(在循环中可用list_del(pos),不会导致死循环) */
#define list_for_each_safe(pos, n, head) \
	for (pos = (head)->next, n = pos->next; \
        pos != (head); \
		pos = n, n = pos->next)

/**
 * list_for_each_prev_safe - iterate over a list backwards safe against removal of list entry
 * @pos:	the &struct list_head to use as a loop cursor.(当前链表指针)
 * @n:		another &struct list_head to use as temporary storage（前一个链表指针）
 * @head:	the head for your list.
 */
/* 往前遍历所有链表节点(在循环中可用list_del(pos),不会导致死循环) */
#define list_for_each_prev_safe(pos, n, head) \
	for (pos = (head)->prev, n = pos->prev; \
	     pos != (head); \
	     pos = n, n = pos->prev)

/**
 * list_for_each_entry	-	iterate over list of given type
 * @pos:	the type * to use as a loop cursor.(当前结构体指针)
 * @head:	the head for your list.(链表表头)
 * @member:	the name of the list_head within the struct.(结构体中链表成员)
 */
/* 往后遍历结构节点 */
#define list_for_each_entry(pos, head, member) \
	for (pos = list_first_entry(head, typeof(*pos), member); \
	     &pos->member != (head); \
	     pos = list_next_entry(pos, member))

/**
 * list_for_each_entry_reverse - iterate backwards over list of given type.
 * @pos:	the type * to use as a loop cursor.(当前结构体指针)
 * @head:	the head for your list.(链表表头)
 * @member:	the name of the list_head within the struct.(结构体中链表成员)
 */
/* 往前遍历结构节点 */
#define list_for_each_entry_reverse(pos, head, member) \
	for (pos = list_last_entry(head, typeof(*pos), member); \
	     &pos->member != (head); \
	     pos = list_prev_entry(pos, member))

/**
 * list_prepare_entry - prepare a pos entry for use in list_for_each_entry_continue()
 * @pos:	the type * to use as a start point
 * @head:	the head of the list
 * @member:	the name of the list_head within the struct.
 *
 * Prepares a pos entry for use as a start point in list_for_each_entry_continue().
 */
#define list_prepare_entry(pos, head, member) \
	((pos) ? : list_entry(head, typeof(*pos), member))

/**
 * list_for_each_entry_continue - continue iteration over list of given type
 * @pos:	the type * to use as a loop cursor.
 * @head:	the head for your list.
 * @member:	the name of the list_head within the struct.
 *
 * Continue to iterate over list of given type, continuing after
 * the current position.
 */
#define list_for_each_entry_continue(pos, head, member) \
	for (pos = list_next_entry(pos, member); \
	     &pos->member != (head); \
	     pos = list_next_entry(pos, member))

/**
 * list_for_each_entry_continue_reverse - iterate backwards from the given point
 * @pos:	the type * to use as a loop cursor.
 * @head:	the head for your list.
 * @member:	the name of the list_head within the struct.
 *
 * Start to iterate over list of given type backwards, continuing after
 * the current position.
 */
#define list_for_each_entry_continue_reverse(pos, head, member) \
	for (pos = list_prev_entry(pos, member); \
	     &pos->member != (head); \
	     pos = list_prev_entry(pos, member))

/**
 * list_for_each_entry_from - iterate over list of given type from the current point
 * @pos:	the type * to use as a loop cursor.
 * @head:	the head for your list.
 * @member:	the name of the list_head within the struct.
 *
 * Iterate over list of given type, continuing from current position.
 */
#define list_for_each_entry_from(pos, head, member) \
	for (; &pos->member != (head); \
	     pos = list_next_entry(pos, member))

/**
 * list_for_each_entry_from_reverse - iterate backwards over list of given type
 *                                    from the current point
 * @pos:	the type * to use as a loop cursor.
 * @head:	the head for your list.
 * @member:	the name of the list_head within the struct.
 *
 * Iterate backwards over list of given type, continuing from current position.
 */
#define list_for_each_entry_from_reverse(pos, head, member) \
	for (; &pos->member != (head); \
	     pos = list_prev_entry(pos, member))

/**
 * list_for_each_entry_safe - iterate over list of given type safe against removal of list entry
 * @pos:	the type * to use as a loop cursor.
 * @n:		another type * to use as temporary storage
 * @head:	the head for your list.
 * @member:	the name of the list_head within the struct.
 */
#define list_for_each_entry_safe(pos, n, head, member) \
	for (pos = list_first_entry(head, typeof(*pos), member), \
		n = list_next_entry(pos, member); \
	     &pos->member != (head); \
	     pos = n, n = list_next_entry(n, member))

/**
 * list_for_each_entry_safe_continue - continue list iteration safe against removal
 * @pos:	the type * to use as a loop cursor.
 * @n:		another type * to use as temporary storage
 * @head:	the head for your list.
 * @member:	the name of the list_head within the struct.
 *
 * Iterate over list of given type, continuing after current point,
 * safe against removal of list entry.
 */
#define list_for_each_entry_safe_continue(pos, n, head, member) \
	for (pos = list_next_entry(pos, member), \
		n = list_next_entry(pos, member); \
	     &pos->member != (head); \
	     pos = n, n = list_next_entry(n, member))

/**
 * list_for_each_entry_safe_from - iterate over list from current point safe against removal
 * @pos:	the type * to use as a loop cursor.
 * @n:		another type * to use as temporary storage
 * @head:	the head for your list.
 * @member:	the name of the list_head within the struct.
 *
 * Iterate over list of given type from current point, safe against
 * removal of list entry.
 */
#define list_for_each_entry_safe_from(pos, n, head, member) \
	for (n = list_next_entry(pos, member); \
	     &pos->member != (head); \
	     pos = n, n = list_next_entry(n, member))

/**
 * list_for_each_entry_safe_reverse - iterate backwards over list safe against removal
 * @pos:	the type * to use as a loop cursor.
 * @n:		another type * to use as temporary storage
 * @head:	the head for your list.
 * @member:	the name of the list_head within the struct.
 *
 * Iterate backwards over list of given type, safe against removal
 * of list entry.
 */
#define list_for_each_entry_safe_reverse(pos, n, head, member) \
	for (pos = list_last_entry(head, typeof(*pos), member), \
		n = list_prev_entry(pos, member); \
	     &pos->member != (head); \
	     pos = n, n = list_prev_entry(n, member))

/**
 * list_safe_reset_next - reset a stale list_for_each_entry_safe loop
 * @pos:	the loop cursor used in the list_for_each_entry_safe loop
 * @n:		temporary storage used in list_for_each_entry_safe
 * @member:	the name of the list_head within the struct.
 *
 * list_safe_reset_next is not safe to use in general if the list may be
 * modified concurrently (eg. the lock is dropped in the loop body). An
 * exception to this is if the cursor element (pos) is pinned in the list,
 * and list_safe_reset_next is called after re-taking the lock and before
 * completing the current iteration of the loop body.
 */
#define list_safe_reset_next(pos, n, member) \
	n = list_next_entry(pos, member)

/*
 * Double linked lists with a single pointer list head.
 * Mostly useful for hash tables where the two pointer list head is
 * too wasteful.
 * You lose the ability to access the tail in O(1).
 */
struct hlist_head {
	struct hlist_node * first;
};

struct hlist_node {
	struct hlist_node * next, ** pprev;
};

#define HLIST_HEAD(name) \
	struct hlist_head name = { .first = NULL }

static inline void init_hlist_head(struct hlist_head * hlist)
{
	hlist->first = NULL;
}

static inline void init_hlist_node(struct hlist_node * h)
{
	h->next = NULL;
	h->pprev = NULL;
}

static inline int hlist_unhashed(const struct hlist_node * h)
{
	return !h->pprev;
}

static inline int hlist_empty(const struct hlist_head * h)
{
	return !h->first;
}

static inline void __hlist_del(struct hlist_node * n)
{
	struct hlist_node * next = n->next;
	struct hlist_node ** pprev = n->pprev;

	*pprev = next;
	if(next)
		next->pprev = pprev;
}

static inline void hlist_del(struct hlist_node * n)
{
	__hlist_del(n);
	n->next = 0;
	n->pprev = 0;
}

static inline void hlist_del_init(struct hlist_node * n)
{
	if(!hlist_unhashed(n))
	{
		__hlist_del(n);
		init_hlist_node(n);
	}
}

static inline void hlist_add_head(struct hlist_node * n, struct hlist_head * h)
{
	struct hlist_node * first = h->first;
	n->next = first;
	if(first)
		first->pprev = &n->next;
	h->first = n;
	n->pprev = &h->first;
}

/* next must be != NULL */
static inline void hlist_add_before(struct hlist_node * n,
					struct hlist_node * next)
{
	n->pprev = next->pprev;
	n->next = next;
	next->pprev = &n->next;
	*(n->pprev) = n;
}

static inline void hlist_add_behind(struct hlist_node * n,
				    struct hlist_node * prev)
{
	n->next = prev->next;
	prev->next = n;
	n->pprev = &prev->next;

	if(n->next)
		n->next->pprev = &n->next;
}

/* after that we'll appear to be on some hlist and hlist_del will work */
static inline void hlist_add_fake(struct hlist_node * n)
{
	n->pprev = &n->next;
}

static inline int hlist_fake(struct hlist_node * h)
{
	return h->pprev == &h->next;
}

/*
 * Check whether the node is the only node of the head without
 * accessing head:
 */
static inline int hlist_is_singular_node(struct hlist_node * n, struct hlist_head * h)
{
	return !n->next && n->pprev == &h->first;
}

/*
 * Move a list from one list head to another. Fixup the pprev
 * reference of the first entry if it exists.
 */
static inline void hlist_move_list(struct hlist_head * old,
				   struct hlist_head * new)
{
	new->first = old->first;
	if(new->first)
		new->first->pprev = &new->first;
	old->first = NULL;
}

#define hlist_entry(ptr, type, member) \
	container_of(ptr, type, member)

#define hlist_for_each(pos, head) \
	for (pos = (head)->first; pos; pos = pos->next)

#define hlist_for_each_safe(pos, n, head) \
	for (pos = (head)->first; pos && ({ n = pos->next; 1; }); \
	     pos = n)

#define hlist_entry_safe(ptr, type, member) \
	({ typeof(ptr) ____ptr = (ptr); \
	   ____ptr ? hlist_entry(____ptr, type, member) : NULL; \
	})

/**
 * hlist_for_each_entry	- iterate over list of given type
 * @pos:	the type * to use as a loop cursor.
 * @head:	the head for your list.
 * @member:	the name of the hlist_node within the struct.
 */
#define hlist_for_each_entry(pos, head, member) \
	for (pos = hlist_entry_safe((head)->first, typeof(*(pos)), member); \
	     pos; \
	     pos = hlist_entry_safe((pos)->member.next, typeof(*(pos)), member))

/**
 * hlist_for_each_entry_continue - iterate over a hlist continuing after current point
 * @pos:	the type * to use as a loop cursor.
 * @member:	the name of the hlist_node within the struct.
 */
#define hlist_for_each_entry_continue(pos, member) \
	for (pos = hlist_entry_safe((pos)->member.next, typeof(*(pos)), member); \
	     pos; \
	     pos = hlist_entry_safe((pos)->member.next, typeof(*(pos)), member))

/**
 * hlist_for_each_entry_from - iterate over a hlist continuing from current point
 * @pos:	the type * to use as a loop cursor.
 * @member:	the name of the hlist_node within the struct.
 */
#define hlist_for_each_entry_from(pos, member) \
	for (; pos; \
	     pos = hlist_entry_safe((pos)->member.next, typeof(*(pos)), member))

/**
 * hlist_for_each_entry_safe - iterate over list of given type safe against removal of list entry
 * @pos:	the type * to use as a loop cursor.
 * @n:		another &struct hlist_node to use as temporary storage
 * @head:	the head for your list.
 * @member:	the name of the hlist_node within the struct.
 */
#define hlist_for_each_entry_safe(pos, n, head, member) \
	for (pos = hlist_entry_safe((head)->first, typeof(*pos), member); \
	     pos && ({ n = pos->member.next; 1; }); \
	     pos = hlist_entry_safe(n, typeof(*pos), member))

#ifdef __cplusplus
}
#endif

#endif /* __LIST_H__ */
