/*
 * Functions for dealing with linked lists of goodies
 *
 * Advanced Rogue
 * Copyright (C) 1984, 1985 Michael Morgan, Ken Dalka and AT&T
 * All rights reserved.
 *
 * Based on "Rogue: Exploring the Dungeons of Doom"
 * Copyright (C) 1980, 1981 Michael Toy, Ken Arnold and Glenn Wichman
 * All rights reserved.
 *
 * See the file LICENSE.TXT for full copyright and licensing information.
 */

#include <stdlib.h>
#include "curses.h"
#include "rogue.h"

/*
 * detach:
 *	Takes an item out of whatever linked list it might be in
 */

void
_detach(struct linked_list **list, struct linked_list *item)
{
    if (*list == item)
	*list = next(item);
    if (prev(item) != NULL) item->l_prev->l_next = next(item);
    if (next(item) != NULL) item->l_next->l_prev = prev(item);
    item->l_next = NULL;
    item->l_prev = NULL;
}

/*
 * _attach:
 *	add an item to the head of a list
 */

void
_attach(struct linked_list **list, struct linked_list *item)
{
    if (*list != NULL)
    {
	item->l_next = *list;
	(*list)->l_prev = item;
	item->l_prev = NULL;
    }
    else
    {
	item->l_next = NULL;
	item->l_prev = NULL;
    }

    *list = item;
}

/*
 * o_free_list:
 *	Throw the whole object list away
 */

void
_o_free_list(struct linked_list **ptr)
{
    register struct linked_list *item;

    while (*ptr != NULL)
    {
	item = *ptr;
	*ptr = next(item);
	o_discard(item);
    }
}

/*
 * o_discard:
 *	free up an item and its object(and maybe contents)
 */

void
o_discard(struct linked_list *item)
{
    register struct object *obj;
    obj = OBJPTR(item);
    if (obj->contents != NULL)
	o_free_list(obj->contents);
    total -= 2;
    FREE(item->l_data);
    FREE(item);
}

/*
 * t_free_list:
 *	Throw the whole thing list away
 */

void
_t_free_list(struct linked_list **ptr)
{
    register struct linked_list *item;

    while (*ptr != NULL)
    {
	item = *ptr;
	*ptr = next(item);
	t_discard(item);
    }
}

/*
 * t_discard:
 *	free up an item and its thing
 */

void
t_discard(struct linked_list *item)
{
    total -= 2;
	if ((struct thing *)item->l_data == shk)
		shk = NULL;
    FREE(item->l_data);
    FREE(item);
}

/*
 * destroy_item:
 *	get rid of an item structure -- don't worry about contents
 */

void
destroy_item(struct linked_list *item)
{
    total--;
    FREE(item);
}

/*
 * new_item
 *	get a new item with a specified size
 */

struct linked_list *
new_item(int size)
{
    register struct linked_list *item;

    if ((item = (struct linked_list *) new(sizeof *item)) == NULL)
	msg("Ran out of memory for header after %d items", total);
    if ((item->l_data = new(size)) == NULL)
	msg("Ran out of memory for data after %d items", total);
    item->l_next = item->l_prev = NULL;
    return item;
}

/*
 * creat_item:
 *	Create just an item structure -- don't make any contents
 */

struct linked_list *
creat_item(void)
{
    register struct linked_list *item;

    if ((item = (struct linked_list *) new(sizeof *item)) == NULL)
	msg("Ran out of memory for header after %d items", total);
    item->l_next = item->l_prev = NULL;
    return item;
}

char *
new(size_t size)
{
    register char *space = ALLOC(size);
    static char errbuf[LINELEN];

    if (space == NULL) {
	sprintf(errbuf,"Rogue ran out of memory (used = %ld, wanted = %d).",
		md_memused(), size);
	fatal(errbuf);
    }
    total++;
    return space;
}
