/*
 * maze
 *
 * Advanced Rogue
 * Copyright (C) 1984, 1985 Michael Morgan, Ken Dalka and AT&T
 * All rights reserved.
 *
 * Based on "Super-Rogue"
 * Copyright (C) 1984 Robert D. Kindelberger
 * All rights reserved.
 *
 * See the file LICENSE.TXT for full copyright and licensing information.
 */

#include <stdlib.h>
#include "curses.h"
#include "rogue.h"

struct cell {
	int y_pos;
	int x_pos;
};

struct bordercells {
	int num_pos;		/* number of frontier cells next to you */
	struct cell conn[4];	/* the y,x position of above cell */
} border_cells;

static char	*frontier, 
		*bits;
static int	lines, 
		cols;
char		*moffset(), 
		*foffset();


/*
 * crankout:
 *	Does actual drawing of maze to window
 */
void
crankout()
{
	reg int x, y;

	for (y = 0; y < MY_LINES - 3; y++) {
		move(y + 1, 0);
		for (x = 0; x < MY_COLS - 1; x++) {
			if (*moffset(y, x)) {		/* here is a wall */
				if(y==0 || y==MY_LINES-4) /* top or bottom line */
					addch('-');
				else if(x==0 || x==MY_COLS-2) /* left | right side */
					addch('|');
				else if (y % 2 == 0 && x % 2 == 0) {
					if(*moffset(y, x-1) || *moffset(y, x+1))
						addch('-');
					else
						addch('|');
				}
				else if (y % 2 == 0)
					addch('-');
				else
					addch('|');
			}
			else
				addch(FLOOR);
		}
	}
}

/*
 * domaze:
 *	Draw the maze on this level.
 */
void
do_maze()
{
	reg int least;
	reg struct room *rp;
	reg struct linked_list *item;
	reg struct object *obj;
	int cnt;
	int treas;
	coord tp;

	for (rp = rooms; rp < &rooms[MAXROOMS]; rp++) {
		rm_turn_on(rp, ISGONE); 	/* kill all rooms */
		rp->r_fires = NULL;		/* no fires */
	}
	rp = &rooms[0];				/* point to only room */
	rm_turn_on(rp, ISDARK);			/* mazes always dark */
	rp->r_pos.x = 0;			/* room fills whole screen */
	rp->r_pos.y = 1;
	rp->r_max.x = MY_COLS - 1;
	rp->r_max.y = MY_LINES - 3;
	draw_maze();				/* put maze into window */
	/*
	 * add some gold to make it worth looking for 
	 */
	item = spec_item(GOLD, 0, 0, 0);
	obj = OBJPTR(item);
	obj->o_count *= (rnd(10) + 1);		/* add in one large hunk */
	attach(lvl_obj, item);
	cnt = 0;
	do {
	    rnd_pos(rp, &tp);
	} until (mvinch(tp.y, tp.x) == FLOOR || cnt++ > 5000);
	mvaddch(tp.y, tp.x, GOLD);
	obj->o_pos = tp;
	/*
	 * add in some food to make sure he has enough
	 */
	item = spec_item(FOOD, 0, 0, 0);
	obj = OBJPTR(item);
	attach(lvl_obj, item);
	do {
	    rnd_pos(rp, &tp);
	} until (mvinch(tp.y, tp.x) == FLOOR || cnt++ > 5000);
	mvaddch(tp.y, tp.x, FOOD);
	obj->o_pos = tp;
	if (rnd(100) < 10) {			/* 10% for treasure maze */
		treas = TRUE;
		least = 6;
		debug("treasure maze");
	}
	else {					/* normal maze level */
		least = 1;
		treas = FALSE;
	}
	genmonsters(least, treas);
}


/*
 * draw_maze:
 *	Generate and draw the maze on the screen
 */
void
draw_maze()
{
	reg int i, j, more;
	reg char *ptr;

	lines = (MY_LINES - 3) / 2;
	cols = (MY_COLS - 1) / 2;
	bits = ALLOC((MY_LINES - 3) * (MY_COLS - 1));
	frontier = ALLOC(lines * cols);
	ptr = frontier;
	while (ptr < (frontier + (lines * cols)))
		*ptr++ = TRUE;
	for (i = 0; i < MY_LINES - 3; i++) {
		for (j = 0; j < MY_COLS - 1; j++) {
			if (i % 2 == 1 && j % 2 == 1)
				*moffset(i, j) = FALSE;		/* floor */
			else
				*moffset(i, j) = TRUE;		/* wall */
		}
	}
	for (i = 0; i < lines; i++) {
		for (j = 0; j < cols; j++) {
			do
				more = findcells(i,j);
			while(more != 0);
		}
	}
	crankout();
	FREE(frontier);
	FREE(bits);
}

/*
 * findcells:
 *	Figure out cells to open up 
 */

int
findcells(int y, int x)
{
	reg int rtpos, i;

	*foffset(y, x) = FALSE;
	border_cells.num_pos = 0;
	if (y < lines - 1) {				/* look below */
		if (*foffset(y + 1, x)) {
			border_cells.conn[border_cells.num_pos].y_pos = y + 1;
			border_cells.conn[border_cells.num_pos].x_pos = x;
			border_cells.num_pos += 1;
		}
	}
	if (y > 0) {					/* look above */
		if (*foffset(y - 1, x)) {
			border_cells.conn[border_cells.num_pos].y_pos = y - 1;
			border_cells.conn[border_cells.num_pos].x_pos = x;
			border_cells.num_pos += 1;

		}
	}
	if (x < cols - 1) {				/* look right */
		if (*foffset(y, x + 1)) {
			border_cells.conn[border_cells.num_pos].y_pos = y;
			border_cells.conn[border_cells.num_pos].x_pos = x + 1;
			border_cells.num_pos += 1;
		}
	}
	if (x > 0) {					/* look left */
		if (*foffset(y, x - 1)) {
			border_cells.conn[border_cells.num_pos].y_pos = y;
			border_cells.conn[border_cells.num_pos].x_pos = x - 1;
			border_cells.num_pos += 1;

		}
	}
	if (border_cells.num_pos == 0)		/* no neighbors available */
		return 0;
	else {
		i = rnd(border_cells.num_pos);
		rtpos = border_cells.num_pos - 1;
		rmwall(border_cells.conn[i].y_pos, border_cells.conn[i].x_pos, y, x);
		return rtpos;
	}
}

/*
 * foffset:
 *	Calculate memory address for frontier
 */
char *
foffset(int y, int x)
{
	return (frontier + (y * cols) + x);
}


/*
 * Maze_view:
 *	Returns true if the player can see the specified location within
 *	the confines of a maze (within one column or row)
 */

int
maze_view(int y, int x)
{
    register int start, goal, delta, ycheck = 0, xcheck = 0, absy, absx, see_radius;
    register int row;
    char ch;	/* What we are standing on (or near) */

    /* Get the absolute value of y and x differences */
    absy = hero.y - y;
    absx = hero.x - x;
    if (absy < 0) absy = -absy;
    if (absx < 0) absx = -absx;

    /* If we are standing in a wall, we can see a bit more */
    switch (ch = CCHAR( winat(hero.y, hero.x) )) {
	case '|':
	case '-':
	case WALL:
	case SECRETDOOR:
	case DOOR:
	    see_radius = 2;
	otherwise:
	    see_radius = 1;
    }

    /* Must be within one or two rows or columns */
    if (absy > see_radius && absx > see_radius) return(FALSE);

    if (absx > see_radius) {		/* Go along row */
	start = hero.x;
	goal = x;
	ycheck = hero.y;
	row = TRUE;
    }
    else {			/* Go along column */
	start = hero.y;
	goal = y;
	xcheck = hero.x;
	row = FALSE;
    }

    if (start <= goal) delta = 1;
    else delta = -1;

    /* Start one past where we are standing */
    if (start != goal) start += delta;

    /* If we are in a wall, we want to look in the area outside the wall */
    if (see_radius > 1) {
	if (row) {
	    /* See if above us it okay first */
	    switch (winat(ycheck, start)) {
		case '|':
		case '-':
		case WALL:
		case DOOR:
		case SECRETDOOR:
		    /* No good, try one up */
		    if (y > hero.y) ycheck++;
		    else ycheck--;
		otherwise:
		    see_radius = 1;	/* Just look straight over the row */
	    }
	}
	else {
	    /* See if above us it okay first */
	    switch (winat(start, xcheck)) {
		case '|':
		case '-':
		case WALL:
		case DOOR:
		case SECRETDOOR:
		    /* No good, try one over */
		    if (x > hero.x) xcheck++;
		    else xcheck--;
		otherwise:
		    see_radius = 1;	/* Just look straight up the column */
	    }
	}
    }

    /* Check boundary again */
    if (absy > see_radius && absx > see_radius) return(FALSE);

    while (start != goal) {
	if (row) xcheck = start;
	else ycheck = start;
	switch (winat(ycheck, xcheck)) {
	    case '|':
	    case '-':
	    case WALL:
	    case DOOR:
	    case SECRETDOOR:
		return(FALSE);
	}
	start += delta;
    }
    return(TRUE);
}


/*
 * moffset:
 *	Calculate memory address for bits
 */
char *
moffset(int y, int x)
{
	return (bits + (y * (MY_COLS - 1)) + x);
}


/*
 * rmwall:
 *	Removes appropriate walls from the maze
 */
void
rmwall(int newy, int newx, int oldy, int oldx)
{
	reg int xdif,ydif;
	
	xdif = newx - oldx;
	ydif = newy - oldy;

	*moffset((oldy * 2) + ydif + 1, (oldx * 2) + xdif + 1) = FALSE;
	findcells(newy, newx);
}
