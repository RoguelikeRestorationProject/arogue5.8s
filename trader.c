/*
 * Anything to do with trading
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

#include <string.h>
#include "curses.h"
#include "rogue.h"

/*
 * buy_it:
 *	Buy the item on which the hero stands
 */
void
buy_it()
{
	struct linked_list *item;
	struct object *obj;
	int wo;

	if (purse <= 0) {
	    msg("You have no money.");
	    return;
	}

	if ((item = get_item(pack, "buy", ALL)) == NULL)
	    return;
	obj = OBJPTR(item);
	if (!(obj->o_flags & ISUNPAID)) {
		msg("You already own that.");
		return;
	}
	wo = get_worth(obj);

	if (wo > purse) {
	    msg("You can't afford that!");
	    msg("It costs %d pieces of gold.", wo);
	    return;
	}
	purse-=wo;
	not_paid--;
	obj->o_flags &= ~ISUNPAID;
}

/*
 * get_worth:
 *	Calculate an objects worth in gold
 */

int
get_worth(struct object *obj)
{
	int worth, wh;

	worth = 0;
	wh = obj->o_which;
	switch (obj->o_type) {
	    case FOOD:
		worth = 125;
	    when WEAPON:
		if (wh < MAXWEAPONS) {
		    worth = weaps[wh].w_worth;
		    worth *= 2*(10 + (obj->o_hplus + obj->o_dplus)/2);
		}
	    when ARMOR:
		if (wh < MAXARMORS) {
		    worth = armors[wh].a_worth;
		    worth *= (4 + ((armors[wh].a_class - obj->o_ac))/2);
		}
	    when SCROLL:
		if (wh < MAXSCROLLS)
		    worth = 2*s_magic[wh].mi_worth;
	    when POTION:
		if (wh < MAXPOTIONS)
		    worth = 2*p_magic[wh].mi_worth;
	    when RING:
		if (wh < MAXRINGS) {
		    worth = 2*r_magic[wh].mi_worth;
		    worth += obj->o_ac * 80;
		}
	    when STICK:
		if (wh < MAXSTICKS) {
		    worth = 2*ws_magic[wh].mi_worth;
		    worth += 40 * obj->o_charges;
		}
	    when MM:
		if (wh < MAXMM) {
		    worth = m_magic[wh].mi_worth;
		    switch (wh) {
			case MM_BRACERS:	worth += 40  * obj->o_ac;
			when MM_PROTECT:	worth += 60  * obj->o_ac;
			when MM_DISP:		/* ac already figured in price*/
			otherwise:		worth += 20  * obj->o_ac;
		    }
		}
	    when RELIC:
		if (wh < MAXRELIC) {
		    worth = rel_magic[wh].mi_worth;
		    if (wh == quest_item) worth *= 10;
		}
	    otherwise:
		worth = 0;
	}
	if (obj->o_flags & ISPROT)	/* 300% more for protected */
	    worth *= 3;
	if (obj->o_flags &  ISBLESSED)	/* 50% more for blessed */
	    worth = worth * 3 / 2;
	if (obj->o_flags & ISCURSED)	/* half for cursed */
	    worth /= 2;
	if (worth < 25)
		worth = 25;
	return(worth);
}

/*
 * price_it:
 *	Price the object that the hero stands on
 */
void
price_it()
{
	reg struct linked_list *item;
	reg struct object *obj;
	reg int wo;


	if ((item = get_item(pack, "price", ALL)) == NULL)
	    return;
	obj = OBJPTR(item);
	if (!(obj->o_flags & ISUNPAID)) {
		msg("You already own that.");
		return;
	}
	wo = get_worth(obj);
	msg("Your %s costs %d pieces of gold.",typ_name(obj),wo);
}

/*
 * typ_name:
 * 	Return the name for this type of object
 */
char *
typ_name(struct object *obj)
{
	static char buff[20];
	reg int wh;

	switch (obj->o_type) {
		case POTION:  wh = TYP_POTION;
		when SCROLL:  wh = TYP_SCROLL;
		when STICK:   wh = TYP_STICK;
		when RING:    wh = TYP_RING;
		when ARMOR:   wh = TYP_ARMOR;
		when WEAPON:  wh = TYP_WEAPON;
		when MM:      wh = TYP_MM;
		when FOOD:    wh = TYP_FOOD;
		when RELIC:   wh = TYP_RELIC;
		otherwise:    wh = -1;
	}
	if (wh < 0)
		strcpy(buff,"unknown");
	else
		strcpy(buff,things[wh].mi_name);
	return (buff);
}


