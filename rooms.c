/*
 * Draw the nine rooms on the screen
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

void
do_rooms()
{
    register int i;
    register struct room *rp;
    register struct linked_list *item;
    register struct thing *tp;
    register int left_out;
    coord top;
    coord bsze;
    coord mp;

    /*
     * bsze is the maximum room size
     */
    bsze.x = MY_COLS/3;
    bsze.y = (MY_LINES-2)/3;
    /*
     * Clear things for a new level
     */
    for (rp = rooms; rp < &rooms[MAXROOMS]; rp++)
    {
	rp->r_flags[0] = 0;
	rp->r_flags[1] = 0;
	rp->r_flags[2] = 0;
	rp->r_flags[3] = 0;
	rp->r_fires = NULL;
    }
    /*
     * Put the gone rooms, if any, on the level
     */
    left_out = rnd(4);
    for (i = 0; i < left_out; i++)
	rm_turn_on(&(rooms[rnd_room()]), ISGONE);
    /*
     * dig and populate all the rooms on the level
     */
    for (i = 0, rp = rooms; i < MAXROOMS; rp++, i++)
    {
	int has_gold=FALSE;

	/*
	 * Find upper left corner of box that this room goes in
	 */
	top.x = (i%3)*bsze.x;
	top.y = i/3*bsze.y + 1;
	if (rm_on(rp, ISGONE))
	{
	    /*
	     * Place a gone room.  Make certain that there is a blank line
	     * for passage drawing.
	     */
	    do
	    {
		rp->r_pos.x = top.x + rnd(bsze.x-2) + 1;
		rp->r_pos.y = top.y + rnd(bsze.y-2) + 1;
		rp->r_max.x = -MY_COLS;
		rp->r_max.x = -MY_LINES;
	    } until(rp->r_pos.y > 0 && rp->r_pos.y < MY_LINES-2);
	    continue;
	}
	if (rnd(10) < level-1)
	    rm_turn_on(rp, ISDARK);
	/*
	 * Find a place and size for a random room
	 */
	do
	{
	    rp->r_max.x = rnd(bsze.x - 4) + 4;
	    rp->r_max.y = rnd(bsze.y - 4) + 4;
	    rp->r_pos.x = top.x + rnd(bsze.x - rp->r_max.x);
	    rp->r_pos.y = top.y + rnd(bsze.y - rp->r_max.y);
	} until (rp->r_pos.y != 0);

	/* Draw the room */
	draw_room(rp);

	/*
	 * Put the gold in
	 */
	if (rnd(100) < 50 && level >= cur_max)
	{
	    register struct linked_list *item;
	    register struct object *cur;
	    coord tp;

	    if ((rm_off(rp, ISTREAS)) &&
		(rm_off(rp, ISSHOP))) {

	    	has_gold = TRUE;	/* This room has gold in it */

	    	item = spec_item(GOLD, 0, 0, 0);
	    	cur = OBJPTR(item);

	    	/* Put the gold into the level list of items */
	    	attach(lvl_obj, item);
	
	    	/* Put it somewhere */
	    	rnd_pos(rp, &tp);
	    	mvaddch(tp.y, tp.x, GOLD);
	    	cur->o_pos = tp;
	    	if (roomin(&tp) != rp) {
			endwin();
			abort();
	    	}
	    }
	}

	/*
	 * Put the monster in
	 */
	if (rnd(100) < (has_gold ? 80 : 25) + vlevel/2)
	{
	    item = new_item(sizeof *tp);
	    tp = THINGPTR(item);
	    do
	    {
		rnd_pos(rp, &mp);
	    } until(mvwinch(stdscr, mp.y, mp.x) == FLOOR);
	    new_monster(item, randmonster(FALSE, FALSE), &mp, FALSE);
	    /*
	     * See if we want to give it a treasure to carry around.
	     */
	    carry_obj(tp, monsters[tp->t_index].m_carry);

	    /*
	     * If it has a fire, mark it
	     */
	    if (on(*tp, HASFIRE)) {
		register struct linked_list *fire_item;

		fire_item = creat_item();
		ldata(fire_item) = (char *) tp;
		attach(rp->r_fires, fire_item);
		rm_turn_on(rp, HASFIRE);
	    }
	}
    }
}


/*
 * Draw a box around a room
 */

void
draw_room(struct room *rp)
{
    register int j, k;

    move(rp->r_pos.y, rp->r_pos.x+1);
    vert(rp->r_max.y-2);				/* Draw left side */
    move(rp->r_pos.y+rp->r_max.y-1, rp->r_pos.x);
    horiz(rp->r_max.x);					/* Draw bottom */
    move(rp->r_pos.y, rp->r_pos.x);
    horiz(rp->r_max.x);					/* Draw top */
    vert(rp->r_max.y-2);				/* Draw right side */
    /*
     * Put the floor down
     */
    for (j = 1; j < rp->r_max.y-1; j++)
    {
	move(rp->r_pos.y + j, rp->r_pos.x+1);
	for (k = 1; k < rp->r_max.x-1; k++)
	    addch(FLOOR);
    }
}

/*
 * horiz:
 *	draw a horizontal line
 */

void
horiz(int cnt)
{
    while (cnt--)
	addch('-');
}

/*
 * rnd_pos:
 *	pick a random spot in a room
 */

void
rnd_pos(struct room *rp, coord *cp)
{
    cp->x = rp->r_pos.x + rnd(rp->r_max.x-2) + 1;
    cp->y = rp->r_pos.y + rnd(rp->r_max.y-2) + 1;
}



/*
 * roomin:
 *	Find what room some coordinates are in. NULL means they aren't
 *	in any room.
 */

struct room *
roomin(coord *cp)
{
    register struct room *rp;

    for (rp = rooms; rp < &rooms[MAXROOMS]; rp++)
	if (inroom(rp, cp))
	    return rp;
    return NULL;
}

/*
 * vert:
 *	draw a vertical line
 */

void
vert(int cnt)
{
    register int x, y;

    getyx(stdscr, y, x);
    x--;
    while (cnt--) {
	move(++y, x);
	addch('|');
    }
}
