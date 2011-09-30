/*
 * all sorts of miscellaneous routines
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

#include <string.h>
#include "curses.h"
#include "rogue.h"
#include <ctype.h>

/*
 * aggravate:
 *	aggravate all the monsters on this level
 */

void
aggravate()
{
    register struct linked_list *mi;

    for (mi = mlist; mi != NULL; mi = next(mi))
	runto(THINGPTR(mi), &hero);
}

/*
 * cansee:
 *	returns true if the hero can see a certain coordinate.
 */

int
cansee(int y, int x)
{
    register struct room *rer;
    register int radius;
    coord tp;

    if (on(player, ISBLIND))
	return FALSE;

    tp.y = y;
    tp.x = x;
    rer = roomin(&tp);

    /* How far can we see? */
    if (levtype == OUTSIDE) {
	if (daytime) radius = 36;
	else if (lit_room(rer)) radius = 9;
	else radius = 3;
    }
    else radius = 3;

    /*
     * We can only see if the hero in the same room as
     * the coordinate and the room is lit or if it is close.
     */
    return ((rer != NULL && 
	     levtype != OUTSIDE &&
	     (levtype != MAZELEV ||	/* Maze level needs direct line */
	      maze_view(tp.y, tp.x)) &&
	     rer == roomin(&hero) &&
	     lit_room(rer)) ||
	    DISTANCE(y, x, hero.y, hero.x) < radius);
}

/*
 * check_level:
 *	Check to see if the guy has gone up a level.
 *
 *	Return points needed to obtain next level.
 *
 * These are the beginning experience levels for all players.
 * All further experience levels are computed by muliplying by 2
 * up through MAXDOUBLE.
 */
#define MAXDOUBLE 14	/* Maximum number of times score is doubled */
static struct {
    unsigned int base;	/* What it starts out at for doubling */
    unsigned int cap;	/* The maximum before doubling stops */
} e_levels[4] = {
	/* You must change MAXDOUBLE if you change the cap figure */
	{ 90,	1474560 },	/* Fighter */
	{ 130,	2129920 }, 	/* Magician */
	{ 110, 1802240 },	/* cleric */
	{ 75,	1228800 }	/* Thief */
};

unsigned int
check_level(int get_spells)
{
    register int i, j, add = 0;
    register unsigned int exp;
    int retval;	/* Return value */
    int nsides = 0;

    /* See if we are past the doubling stage */
    exp = e_levels[player.t_ctype].cap;
    if (pstats.s_exp >= exp) {
	i = pstats.s_exp/exp;	/* First get amount above doubling area */
	retval = exp + i * exp; /* Compute next higher boundary */
	i += MAXDOUBLE;	/* Add in the previous doubled levels */
    }
    else {
	i = 0;
	exp = e_levels[player.t_ctype].base;
	while (exp <= pstats.s_exp) {
	    i++;
	    exp <<= 1;
	}
	retval = exp;
    }
    if (++i > pstats.s_lvl) {
	switch (player.t_ctype) {
	    case C_FIGHTER:	nsides = 10;
	    when C_MAGICIAN:	nsides = 4;
	    when C_CLERIC:	nsides = 8;
	    when C_THIEF:	nsides = 6;
	}

	/* Take care of multi-level jumps */
	for (j=0; j < (i-pstats.s_lvl); j++)
	    add += max(1, roll(1,nsides) + const_bonus());
	max_stats.s_hpt += add;
	if ((pstats.s_hpt += add) > max_stats.s_hpt)
	    pstats.s_hpt = max_stats.s_hpt;
	msg("Welcome, %s, to level %d",
	    cnames[player.t_ctype][min(i-1, 10)], i);
	if (get_spells) {
	    pray_time = 0;	/* A new round of prayers */
	    spell_power = 0; /* A new round of spells */
	}
    }
    pstats.s_lvl = i;
    return(retval);
}

/*
 * Used to modify the playes strength
 * it keeps track of the highest it has been, just in case
 */

void
chg_str(int amt)
{
    register int ring_str;		/* ring strengths */
    register struct stats *ptr;		/* for speed */

    ptr = &pstats;
    ring_str = ring_value(R_ADDSTR);
    ptr->s_str += ring_str+spinach_strength;
    ptr->s_str += amt;
    if (ptr->s_str > 25)
	ptr->s_str = 25;
    if (ptr->s_str > max_stats.s_str)
	max_stats.s_str = ptr->s_str;
    ptr->s_str += ring_str+spinach_strength;
    if (ptr->s_str <= 0)
	death(D_STRENGTH);
    updpack(TRUE);
}

/*
 * this routine computes the players current AC without dex bonus's
 */
int 
ac_compute()
{
    register int ac;

    ac  = cur_armor != NULL ? cur_armor->o_ac : pstats.s_arm;
    ac -= ring_value(R_PROTECT);
    if (cur_misc[WEAR_BRACERS] != NULL)
	ac -= cur_misc[WEAR_BRACERS]->o_ac;
    if (cur_misc[WEAR_CLOAK] != NULL)
	ac -= cur_misc[WEAR_CLOAK]->o_ac;

    /* If player has the cloak, must be wearing it */
    if (cur_relic[EMORI_CLOAK]) ac -= 5;

    if (cur_gloves != NULL) {
		if (cur_gloves->o_ac <= 0)
			ac += cur_gloves->o_ac/2 - 1;
		else
			ac += 1;
    }
    if (cur_helmet != NULL) {
		if (cur_helmet->o_ac <= 0)
			ac += cur_helmet->o_ac/2 - 1;
		else
			ac += 1;
    }
    if (cur_shield != NULL) {
		if (cur_shield->o_ac <= 0)
			ac += cur_shield->o_ac - 1;
		else
			ac += cur_shield->o_ac;
    }

    if (ac > 10)
	ac = 10;
    return(ac);
}

/*
 * this routine computes the players current strength
 */
int
str_compute()
{
    if (cur_misc[WEAR_GAUNTLET] != NULL		&&
	cur_misc[WEAR_GAUNTLET]->o_which == MM_G_OGRE) {
	if (cur_misc[WEAR_GAUNTLET]->o_flags & ISCURSED)
	    return (3);
	else
	    return (18);
    }
    else
	    return (pstats.s_str);
}

/*
 * this routine computes the players current dexterity
 */
int
dex_compute()
{
    if (cur_misc[WEAR_GAUNTLET] != NULL		&&
	cur_misc[WEAR_GAUNTLET]->o_which == MM_G_DEXTERITY) {
	if (cur_misc[WEAR_GAUNTLET]->o_flags & ISCURSED)
	    return (3);
	else
	    return (18);
    }
    else
	    return (pstats.s_dext);
}
    

/*
 * diag_ok:
 *	Check to see if the move is legal if it is diagonal
 */

int
diag_ok(coord *sp, coord *ep, struct thing *flgptr)
{
    register int numpaths = 0;

    /* Horizontal and vertical moves are always ok */
    if (ep->x == sp->x || ep->y == sp->y)
	return TRUE;

    /* Diagonal moves are not allowed if there is a horizontal or
     * vertical path to the destination
     */
    if (step_ok(ep->y, sp->x, MONSTOK, flgptr)) numpaths++;
    if (step_ok(sp->y, ep->x, MONSTOK, flgptr)) numpaths++;
    return(numpaths != 1);
}

/*
 * eat:
 *	He wants to eat something, so let him try
 */

void
eat()
{
    register struct linked_list *item;
    register struct object *obj;
    int prob, oldfood;
	int save_max;

    if ((item = get_item(pack, "eat", FOOD)) == NULL)
	return;
    obj = (struct object *) ldata(item);
    oldfood = food_left;
    switch (obj->o_which) {
	case 0: /* tin */
		prob = rnd(100);
		if (prob < 30) {
			msg("It contains first quality peaches - what a surprise!");
			food_left += rnd(700);
		}
		else if (prob < 45) {
			msg("It contains salmon - not bad!");
			food_left += rnd(1200);
		}
		else if (prob < 55) {
			msg("It contains apple juice - perhaps not what you hoped for.");
			food_left += rnd(200);
		}
		else if (prob < 65) {
			msg("It contains some nondescript substance, tasting awfully.");
			food_left += rnd(1500);
		}
		else if (prob < 75) {
			msg("It contains rotten meat. You vomit.");
			food_left -= rnd(1000);
		}
		else if (prob < 90) {
			msg("It contains spinach - this makes you feel like Popeye!");
			if (spinach_strength == 0) {
				if (lost_str != 0) {
					res_strength();
				}
				else {
					spinach_strength = rnd(5) + 1;
					save_max = max_stats.s_str;
					chg_str(spinach_strength);
					max_stats.s_str = save_max;
					fuse(spinach_str, 0, 250*CHILLTIME, AFTER);
				}
			}
			else {
				chg_str(-1*spinach_strength);
				spinach_strength += (rnd(5) + 1);
				save_max = max_stats.s_str;
				chg_str(spinach_strength);
				max_stats.s_str = save_max;
				lengthen(spinach_str, 250*CHILLTIME);
			}
			food_left += rnd(2000);
		}
		else {
			msg("It turns out to be empty.");
		}
		break;
	case 1: /* fruit */
    		food_left += HUNGERTIME + rnd(400) - 200;
		if (food_left < 3000)
			msg("My, that was a yummy %s", fruit);
    		updpack(TRUE);
    		if (obj == cur_weapon)
			cur_weapon = NULL;
		break;
	case 2: /* food ration */
    		food_left += HUNGERTIME + rnd(400) - 200;
    		updpack(TRUE);
    		if (obj == cur_weapon)
			cur_weapon = NULL;
		if (rnd(100) > 70)
		{
			if (food_left < 3000)
	    			msg("Yuk, this food tastes awful");
	    		pstats.s_exp++;
	    		check_level(0);
		}
		else
			if (food_left < 3000)
	    			msg("Yum, that tasted good");
		break;
	case 3: /* carrot */
		if (on(player, ISBLIND))
		{
			extinguish(sight);
			turn_off(player, ISBLIND);
			light(&hero);
			msg("The veil of darkness lifts");
		}
		else if (off(player, CANSEE)) {
			turn_on(player, CANSEE);
			msg("Your eyes begin to tingle.");
			fuse(unsee, 0, SEEDURATION*3, AFTER);
			light(&hero);
		}
		else if (find_slot(unsee) != 0)
			lengthen(unsee, SEEDURATION*3);
		break;
	case 4: /* lump of royal jelly */
		if (rnd(100) > 40) {
			if ((pstats.s_hpt += roll(pstats.s_lvl, 8)) > max_stats.s_hpt)
				pstats.s_hpt = ++max_stats.s_hpt;
			if (on(player, ISHUH)) {
				extinguish(unconfuse);
				unconfuse();
			}
		}
		if (rnd(100) > 40) {
			msg("A strong blue aura surrounds your head.");
			turn_on(player, ISCLEAR);
		}
		if (rnd(100) > 50) 
			raise_level(0);
		if (rnd(100) > 50) 
			raise_level(0);
		if (rnd(100) > 50)
			add_intelligence(FALSE);
		if (rnd(100) > 30)
			add_dexterity(FALSE);
		if (rnd(100) > 30)
			add_strength(FALSE);
		if (rnd(100) > 30)
			add_wisdom(FALSE);
		if (rnd(100) > 30)
			add_const(FALSE);
		break;
    }
    del_pack(item);
    if (food_left >= 5000) {
	msg("You choke over your food.");
	death(D_FCHOKE);
    }
    else if (food_left >= 4000 && oldfood < 4000) {
	msg("You're having a hard time getting all that food down.");
    }
    else if (food_left >= 3000 && oldfood < 3000) {
	msg("That satiated your stomach!");
    }
    stomach();
}

/*
 * pick a random position around the give (y, x) coordinates
 */
coord *
fallpos(coord *pos, int be_clear, int range)
{
	register int tried, i, j;
	register char ch;
	static coord ret;
	static short masks[] = {
		0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x100 };

/*
 * Pick a spot at random centered on the position given by 'pos' and
 * up to 'range' squares away from 'pos'
 *
 * If 'be_clear' is TRUE, the spot must be either FLOOR or PASSAGE
 * inorder to be considered valid
 *
 *
 * Generate a number from 0 to 8, representing the position to pick.
 * Note that this DOES include the positon 'pos' itself
 *
 * If this position is not valid, mark it as 'tried', and pick another.
 * Whenever a position is picked that has been tried before,
 * sequentially find the next untried position. This eliminates costly
 * random number generation
 */

	tried = 0;
	while( tried != 0x1ff ) {
		i = rnd(9);
		while( tried & masks[i] )
			i = (i + 1) % 9;

		tried |= masks[i];

		for( j = 1; j <= range; j++ ) {
			ret.x = pos->x + j*grid[i].x;
			ret.y = pos->y + j*grid[i].y;

			if (ret.x == hero.x && ret.y == hero.y)
				continue; /* skip the hero */

			if (ret.x < 0 || ret.x > MY_COLS - 1 ||
			    ret.y < 1 || ret.y > MY_LINES - 3)
				continue; /* off the screen? */

			ch = CCHAR( winat(ret.y, ret.x) );

			/*
			 * Check to make certain the spot is valid
			 */
			switch( ch ) {
			case FLOOR:
			case PASSAGE:
				return( &ret );
			case GOLD:
			case SCROLL:
			case POTION:
			case STICK:
			case RING:
			case WEAPON:
			case ARMOR:
			case MM:
			case FOOD:
				if(!be_clear)
					return( &ret );
			default:
				break;
			}
		}
	}
	return( NULL );
}


/*
 * find_mons:
 *	Find the monster from his corrdinates
 */

struct linked_list *
find_mons(int y, int x)
{
    register struct linked_list *item;
    register struct thing *th;

    for (item = mlist; item != NULL; item = next(item))
    {
	th = THINGPTR(item);
	if (th->t_pos.y == y && th->t_pos.x == x)
	    return item;
    }
    return NULL;
}

/*
 * find_obj:
 *	find the unclaimed object at y, x
 */

struct linked_list *
find_obj(int y, int x)
{
    register struct linked_list *obj;
    register struct object *op;

    for (obj = lvl_obj; obj != NULL; obj = next(obj))
    {
	op = OBJPTR(obj);
	if (op->o_pos.y == y && op->o_pos.x == x)
		return obj;
    }
    return NULL;
}


/*
 * set up the direction co_ordinate for use in varios "prefix" commands
 */
int
get_dir()
{
    register char *prompt;
    register int gotit;

    prompt = terse ? "Direction?" :  "Which direction? ";
    msg(prompt);
    do
    {
	gotit = TRUE;
	switch (readchar())
	{
	    case 'h': case'H': delta.y =  0; delta.x = -1;
	    when 'j': case'J': delta.y =  1; delta.x =  0;
	    when 'k': case'K': delta.y = -1; delta.x =  0;
	    when 'l': case'L': delta.y =  0; delta.x =  1;
	    when 'y': case'Y': delta.y = -1; delta.x = -1;
	    when 'u': case'U': delta.y = -1; delta.x =  1;
	    when 'b': case'B': delta.y =  1; delta.x = -1;
	    when 'n': case'N': delta.y =  1; delta.x =  1;
	    when '.':  	       delta.y =  0; delta.x =  0;
	    when ESCAPE: return FALSE;
	    otherwise:
		mpos = 0;
		msg(prompt);
		gotit = FALSE;
	}
    } until (gotit);
    if ((on(player, ISHUH) || on(player, ISDANCE)) && rnd(100) > 20) {
	do
	{
	    delta = grid[rnd(9)];
	} while (delta.y == 0 && delta.x == 0);
    }
    mpos = 0;
    return TRUE;
}

/* 
 * see if the object is one of the currently used items
 */
int
is_current(struct object *obj)
{
    if (obj == NULL)
	return FALSE;
    if (obj == cur_armor		|| obj == cur_weapon		|| 
	obj == cur_shield 		|| obj == cur_gloves            ||
	obj == cur_helmet ||
	obj == cur_ring[LEFT_1]		|| obj == cur_ring[LEFT_2]	||
	obj == cur_ring[LEFT_3]		|| obj == cur_ring[LEFT_4]	||
	obj == cur_ring[RIGHT_1]	|| obj == cur_ring[RIGHT_2]	||
	obj == cur_ring[RIGHT_3]	|| obj == cur_ring[RIGHT_4]	||
	obj == cur_misc[WEAR_BOOTS]	|| obj == cur_misc[WEAR_JEWEL]	||
	obj == cur_misc[WEAR_BRACERS]	|| obj == cur_misc[WEAR_CLOAK]	||
	obj == cur_misc[WEAR_GAUNTLET]	|| obj == cur_misc[WEAR_NECKLACE]) {

	return TRUE;
    }

    /* Is it a "current" relic? */
    if (obj->o_type == RELIC) {
	switch (obj->o_which) {
	    case MUSTY_DAGGER:
	    case EMORI_CLOAK:
	    case HEIL_ANKH:
	    case YENDOR_AMULET:
	    case HRUGGEK_MSTAR:
	    case YEENOGHU_FLAIL:
		if (cur_relic[obj->o_which]) return TRUE;
	}
    }

    return FALSE;
}


/*
 * Look:
 *	A quick glance all around the player
 */

void
look(int wakeup, int runend)
/* wakeup = Should we wake up monsters */
/* runend = At end of a run -- for mazes */
{
    register int x, y, radius;
    register int ch, och;
    register int oldx, oldy;
    register int horiz, vert, do_blank = FALSE;
    register int passcount = 0, curfloorcount = 0, nextfloorcount = 0;
    register struct room *rp;
    register int ey, ex;
	int inpass, do_light = FALSE;

    inpass = ((rp = roomin(&hero)) == NULL); /* Are we in a passage? */

    /* Are we moving vertically or horizontally? */
    if (runch == 'h' || runch == 'l') horiz = TRUE;
    else horiz = FALSE;
    if (runch == 'j' || runch == 'k') vert = TRUE;
    else vert = FALSE;

    /* How far around himself can the player see? */
    if (levtype == OUTSIDE) {
	if (daytime) radius = 6;
	else if (lit_room(rp)) radius = 3;
	else radius = 1;
    }
    else radius = 1;

    getyx(cw, oldy, oldx);	/* Save current position */

    /* Blank out the floor around our last position and check for
     * moving out of a corridor in a maze.
     */
    if (levtype == OUTSIDE) do_blank = !daytime;
    else if (oldrp != NULL && !lit_room(oldrp) && off(player, ISBLIND))
	    do_blank = TRUE;

    /* Now move around the old position and blank things out */
    ey = player.t_oldpos.y + radius;
    ex = player.t_oldpos.x + radius;
    for (x = player.t_oldpos.x - radius; x <= ex; x++)
      if (x >= 0 && x < MY_COLS)
	for (y = player.t_oldpos.y - radius; y <= ey; y++) {
	    char savech;	/* Saves character in monster window */

	    if (y < 1 || y > MY_LINES - 3) continue;

	    /* See what's there -- ignore monsters, just see what they're on */
	    savech = CCHAR( mvwinch(mw, y, x) );
	    waddch(mw, ' ');
	    ch = show(y, x);
	    mvwaddch(mw, y, x, savech);	/* Restore monster */

	    if (do_blank && (y != hero.y || x != hero.x))
		switch (ch) {
		    case DOOR:
		    case SECRETDOOR:
		    case PASSAGE:
		    case STAIRS:
		    case TRAPDOOR:
		    case TELTRAP:
		    case BEARTRAP:
		    case SLEEPTRAP:
		    case ARROWTRAP:
		    case DARTTRAP:
		    case MAZETRAP:
		    case POOL:
		    case '|':
		    case '-':
		    case WALL:
			/* If there was a monster showing, make it disappear */
			if (isalpha(savech)) mvwaddch(cw, y, x, ch);
			break;
		    when FLOOR:
		    case FOREST:
		    default:
			mvwaddch(cw, y, x, ' ');
		}
		
	    /* Moving out of a corridor? */
	    if (levtype == MAZELEV && !ce(hero, player.t_oldpos) &&
		!running && !isrock(ch) &&  /* Not running and not a wall */
		((vert && x != player.t_oldpos.x && y==player.t_oldpos.y) ||
		 (horiz && y != player.t_oldpos.y && x==player.t_oldpos.x)))
		    do_light = off(player, ISBLIND);
	}

    /* Take care of unlighting a corridor */
    if (do_light && lit_room(rp)) light(&player.t_oldpos);

    /* Are we coming or going between a wall and a corridor in a maze? */
    och = show(player.t_oldpos.y, player.t_oldpos.x);
    ch = show(hero.y, hero.x);
    if (levtype == MAZELEV &&
	((isrock(och) && !isrock(ch)) || (isrock(ch) && !isrock(och)))) {
	do_light = off(player, ISBLIND); /* Light it up if not blind */

	/* Unlight what we just saw */
	if (do_light && lit_room(&rooms[0])) light(&player.t_oldpos);
    }

    /* Look around the player */
    ey = hero.y + radius;
    ex = hero.x + radius;
    for (x = hero.x - radius; x <= ex; x++)
	if (x >= 0 && x < MY_COLS) for (y = hero.y - radius; y <= ey; y++) {
	    if (y < 1 || y >= MY_LINES - 2)
		continue;
	    if (isalpha(mvwinch(mw, y, x)))
	    {
		register struct linked_list *it;
		register struct thing *tp;

		if (wakeup)
		    it = wake_monster(y, x);
		else
		    it = find_mons(y, x);
		tp = THINGPTR(it);
		if ((mon_know[tp->t_index] == FALSE) && off(player, ISBLIND)) {
			msg("Beware!  You may not be familiar with the %s nearby.", monsters[tp->t_index].m_name);
			mon_know[tp->t_index] = TRUE;
		}
		tp->t_oldch = CCHAR( mvinch(y, x) );
		if (isatrap(tp->t_oldch)) {
		    register struct trap *trp = trap_at(y, x);

		    tp->t_oldch = (trp->tr_flags & ISFOUND) ? tp->t_oldch
							    : trp->tr_show;
		}
		if (tp->t_oldch == FLOOR && !lit_room(rp) &&
		    off(player, ISBLIND))
			tp->t_oldch = ' ';
	    }

	    /*
	     * Secret doors show as walls
	     */
	    if ((ch = show(y, x)) == SECRETDOOR)
		ch = secretdoor(y, x);
	    /*
	     * Don't show room walls if he is in a passage and
	     * check for maze turns
	     */
	    if (off(player, ISBLIND))
	    {
		if ((y == hero.y && x == hero.x)
		    || (inpass && (ch == '-' || ch == '|')))
			continue;

		/* Did we come to a crossroads in a maze? */
		if (levtype == MAZELEV &&
		    (runend || !ce(hero, player.t_oldpos)) &&
		    !isrock(ch) &&	/* Not a wall */
		    ((vert && x != hero.x && y == hero.y) ||
		     (horiz && y != hero.y && x == hero.x)))
			/* Just came to a turn */
			do_light = off(player, ISBLIND);
	    }
	    else if (y != hero.y || x != hero.x)
		continue;

	    wmove(cw, y, x);
	    waddch(cw, ch);
	    if (door_stop && !firstmove && running)
	    {
		switch (runch)
		{
		    case 'h':
			if (x == hero.x + 1)
			    continue;
		    when 'j':
			if (y == hero.y - 1)
			    continue;
		    when 'k':
			if (y == hero.y + 1)
			    continue;
		    when 'l':
			if (x == hero.x - 1)
			    continue;
		    when 'y':
			if ((x + y) - (hero.x + hero.y) >= 1)
			    continue;
		    when 'u':
			if ((y - x) - (hero.y - hero.x) >= 1)
			    continue;
		    when 'n':
			if ((x + y) - (hero.x + hero.y) <= -1)
			    continue;
		    when 'b':
			if ((y - x) - (hero.y - hero.x) <= -1)
			    continue;
		}
		switch (ch)
		{
		    case DOOR:
			if (x == hero.x || y == hero.y)
			    running = FALSE;
			break;
		    case PASSAGE:
			if (x == hero.x || y == hero.y)
			    passcount++;
			break;
		    case FLOOR:
			/* Stop by new passages in a maze (floor next to us) */
			if ((levtype == MAZELEV) &&
			    !(hero.y == y && hero.x == x)) {
			    if (vert) {	/* Moving vertically */
				/* We have a passage on our row */
				if (y == hero.y) curfloorcount++;

				/* Some passage on the next row */
				else if (y != player.t_oldpos.y)
				    nextfloorcount++;
			    }
			    else {	/* Moving horizontally */
				/* We have a passage on our column */
				if (x == hero.x) curfloorcount++;

				/* Some passage in the next column */
				else if (x != player.t_oldpos.x)
				    nextfloorcount++;
			    }
			}
		    case '|':
		    case '-':
		    case ' ':
			break;
		    default:
			running = FALSE;
			break;
		}
	    }
	}

    /* Have we passed a side passage, with multiple choices? */
    if (curfloorcount > 0 && nextfloorcount > 0) running = FALSE;

    else if (door_stop && !firstmove && passcount > 1)
	running = FALSE;

    /* Do we have to light up the area (just stepped into a new corridor)? */
    if (do_light && !running && lit_room(rp)) light(&hero);

    if (punished) {
		ch = CCHAR( winat(heavy_ball.y, heavy_ball.x) );
		if (!isalpha(ch))
			mvwaddch(cw, heavy_ball.y, heavy_ball.x, HEAVY_BALL);
		else
			mvwaddch(cw, heavy_ball.y, heavy_ball.x, ch);
    }
    mvwaddch(cw, hero.y, hero.x, PLAYER);
    wmove(cw, oldy, oldx);
    if (!ce(player.t_oldpos, hero)) {
	player.t_oldpos = hero; /* Don't change if we didn't move */
	oldrp = rp;
    }
}

/*
 * raise_level:
 *	The guy just magically went up a level.
 */

void
raise_level(int get_spells)
{
    unsigned int test;	/* Next level -- be sure it is not an overflow */

    test = check_level(FALSE);	/* Get next boundary */

    /* Be sure it is higher than what we have now -- else overflow */
    if (test > pstats.s_exp) pstats.s_exp = test;
    check_level(get_spells);
}

/*
 * saving throw matrix for character saving throws
 * this table is indexed by char type and saving throw type
 */
static int st_matrix[5][5] = {
/* Poison,	Petrify,	wand,		Breath,		Magic */
{ 14,		15,		16,		16,		17 },
{ 14,		13,		11,		15,		12 },
{ 10,		13,		14,		16,		15 },
{ 13,		12,		14,		16,		15 },
{ 14,		15,		16,		16,		17 }
};

/*
 * save:
 *	See if a creature saves against something
 */
int
save(int which, struct thing *who, int adj)
/* which = which type of save */
/* who = who is saving */
/* adj = saving throw adjustment */
{
    register int need, level;

    level = who->t_stats.s_lvl;
    need = st_matrix[who->t_ctype][which];
    switch (who->t_ctype) {
    case C_FIGHTER:
	need -= (level-1) / 2;
    when C_MAGICIAN:
	need -= 2 * (level-1) / 5;
    when C_CLERIC:
	need -= (level-1) / 3;
    when C_THIEF:
	need -= 2 * (level-1) / 4;
    when C_MONSTER:
	need -= level / 2;
    }
    /* 
     * add in pluses against poison for execeptional constitution 
     */
    if (which == VS_POISON && who->t_stats.s_const > 18)
	need -= (who->t_stats.s_const - 17) / 2;
    /*
     * does the player have a ring of protection on?
     */
    if (who == &player)
	need -= (min(ring_value(R_PROTECT),3)); /* no more than +3 bonus */
    /*
     * does the player have a cloak of protection on?
     */
    if (who == &player && cur_misc[WEAR_CLOAK])
	need -= (min(cur_misc[WEAR_CLOAK]->o_ac,3)); /* no more than +3 bonus */
    need -= adj;
    debug("need a %d to save", need);
    return (roll(1, 20) >= need);
}


/*
 * secret_door:
 *	Figure out what a secret door looks like.
 */

int
secretdoor(int y, int x)
{
    register int i;
    register struct room *rp;
    register coord *cpp;
    static coord cp;

    cp.y = y;
    cp.x = x;
    cpp = &cp;
    for (rp = rooms, i = 0; i < MAXROOMS; rp++, i++)
	if (inroom(rp, cpp))
	{
	    if (y == rp->r_pos.y || y == rp->r_pos.y + rp->r_max.y - 1)
		return('-');
	    else
		return('|');
	}

    return('p');
}

/*
 * copy string using unctrl for things
 */
void
strucpy(char *s1, char *s2, size_t len)
{
    register char *sp;

    while (len--)
    {
	strcpy(s1, (sp = (char *) unctrl(*s2)));
	s1 += strlen(sp);
        s2++;
    }
    *s1 = '\0';
}

/*
 * tr_name:
 *	print the name of a trap
 */

char *
tr_name(int ch)
{
    register char *s = NULL;

    switch (ch)
    {
	case TRAPDOOR:
	    s = terse ? "A trapdoor." : "You found a trapdoor.";
	when BEARTRAP:
	    s = terse ? "A beartrap." : "You found a beartrap.";
	when SLEEPTRAP:
	    s = terse ? "A sleeping gas trap.":"You found a sleeping gas trap.";
	when ARROWTRAP:
	    s = terse ? "An arrow trap." : "You found an arrow trap.";
	when TELTRAP:
	    s = terse ? "A teleport trap." : "You found a teleport trap.";
	when DARTTRAP:
	    s = terse ? "A dart trap." : "You found a poison dart trap.";
	when POOL:
	    s = terse ? "A shimmering pool." : "You found a shimmering pool";
	when MAZETRAP:
	    s = terse ? "A maze entrance." : "You found a maze entrance";
    }
    return s;
}

/*
 * for printfs: if string starts with a vowel, return "n" for an "an"
 */
char *
vowelstr(char *str)
{
    switch (*str)
    {
	case 'a':
	case 'e':
	case 'i':
	case 'o':
	case 'u':
	    return "n";
	default:
	    return "";
    }
}

/*
 * waste_time:
 *	Do nothing but let other things happen
 */

void
waste_time()
{
    if (inwhgt)			/* if from wghtchk then done */
	return;
    do_daemons(BEFORE);
    do_fuses(BEFORE);
    do_daemons(AFTER);
    do_fuses(AFTER);
}
