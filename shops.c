/*
 * shops, special rooms
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

char *shkliquors[] = {
	/* Ukraine */
	"Njezjin", "Tsjernigof", "Gomel", "Ossipewsk", "Gorlowka",
	/* N. Russia */
	"Konosja", "Weliki Oestjoeg", "Syktywkar", "Sablja",
	"Narodnaja", "Kyzyl",
	/* Silezie */
	"Walbrzych", "Swidnica", "Klodzko", "Raciborz", "Gliwice",
	"Brzeg", "Krnov", "Hradec Kralove",
	/* Schweiz */
	"Leuk", "Brig", "Brienz", "Thun", "Sarnen", "Burglen", "Elm",
	"Flims", "Vals", "Schuls", "Zum Loch",
};
#define NSHKLIQUORS (sizeof shkliquors / sizeof (char *))

char *shkbooks[] = {
	/* Eire */
	"Skibbereen", "Kanturk", "Rath Luirc", "Ennistymon", "Lahinch",
	"Loughrea", "Croagh", "Maumakeogh", "Ballyjamesduff",
	"Kinnegad", "Lugnaquillia", "Enniscorthy", "Gweebarra",
	"Kittamagh", "Nenagh", "Sneem", "Ballingeary", "Kilgarvan",
	"Cahersiveen", "Glenbeigh", "Kilmihil", "Kiltamagh",
	"Droichead Atha", "Inniscrone", "Clonegal", "Lisnaskea",
	"Culdaff", "Dunfanaghy", "Inishbofin", "Kesh",
};
#define NSHKBOOKS (sizeof shkbooks / sizeof (char *))

char *shkarmors[] = {
	/* Turquie */
	"Demirci", "Kalecik", "Boyabai", "Yildizeli", "Gaziantep",
	"Siirt", "Akhalataki", "Tirebolu", "Aksaray", "Ermenak",
	"Iskenderun", "Kadirli", "Siverek", "Pervari", "Malasgirt",
	"Bayburt", "Ayancik", "Zonguldak", "Balya", "Tefenni",
	"Artvin", "Kars", "Makharadze", "Malazgirt", "Midyat",
	"Birecik", "Kirikkale", "Alaca", "Polatli", "Nallihan",
};
#define NSHKARMORS (sizeof shkarmors / sizeof (char *))

char *shkwands[] = {
	/* Wales */
	"Yr Wyddgrug", "Trallwng", "Mallwyd", "Pontarfynach",
	"Rhaeader", "Llandrindod", "Llanfair-ym-muallt",
	"Y-Fenni", "Measteg", "Rhydaman", "Beddgelert",
	"Curig", "Llanrwst", "Llanerchymedd", "Caergybi",
	/* Scotland */
	"Nairn", "Turriff", "Inverurie", "Braemar", "Lochnagar",
	"Kerloch", "Beinn a Ghlo", "Drumnadrochit", "Morven",
	"Uist", "Storr", "Sgurr na Ciche", "Cannich", "Gairloch",
	"Kyleakin", "Dunvegan",
};
#define NSHKWANDS (sizeof shkwands / sizeof (char *))

char *shkrings[] = {
	/* Hollandse familienamen */
	"Feyfer", "Flugi", "Gheel", "Havic", "Haynin", "Hoboken",
	"Imbyze", "Juyn", "Kinsky", "Massis", "Matray", "Moy",
	"Olycan", "Sadelin", "Svaving", "Tapper", "Terwen", "Wirix",
	"Ypey",
	/* Skandinaviske navne */
	"Rastegaisa", "Varjag Njarga", "Kautekeino", "Abisko",
	"Enontekis", "Rovaniemi", "Avasaksa", "Haparanda",
	"Lulea", "Gellivare", "Oeloe", "Kajaani", "Fauske",
};
#define NSHKRINGS (sizeof shkrings / sizeof (char *))

char *shkfoods[] = {
	/* Indonesia */
	"Djasinga", "Tjibarusa", "Tjiwidej", "Pengalengan",
	"Bandjar", "Parbalingga", "Bojolali", "Sarangan",
	"Ngebel", "Djombang", "Ardjawinangun", "Berbek",
	"Papar", "Baliga", "Tjisolok", "Siboga", "Banjoewangi",
	"Trenggalek", "Karangkobar", "Njalindoeng", "Pasawahan",
	"Pameunpeuk", "Patjitan", "Kediri", "Pemboeang", "Tringanoe",
	"Makin", "Tipor", "Semai", "Berhala", "Tegal", "Samoe",
};
#define NSHKFOODS (sizeof shkfoods / sizeof (char *))

char *shkweapons[] = {
	/* Perigord */
	"Voulgezac", "Rouffiac", "Lerignac", "Touverac", "Guizengeard",
	"Melac", "Neuvicq", "Vanzac", "Picq", "Urignac", "Corignac",
	"Fleac", "Lonzac", "Vergt", "Queyssac", "Liorac", "Echourgnac",
	"Cazelon", "Eypau", "Carignan", "Monbazillac", "Jonzac",
	"Pons", "Jumilhac", "Fenouilledes", "Laguiolet", "Saujon",
	"Eymoutiers", "Eygurande", "Eauze", "Labouheyre",
};
#define NSHKWEAPONS (sizeof shkweapons / sizeof (char *))

char *shkgeneral[] = {
	/* Suriname */
	"Hebiwerie", "Possogroenoe", "Asidonhopo", "Manlobbi",
	"Adjama", "Pakka Pakka", "Kabalebo", "Wonotobo",
	"Akalapi", "Sipaliwini",
	/* Greenland */
	"Annootok", "Upernavik", "Angmagssalik",
	/* N. Canada */
	"Aklavik", "Inuvik", "Tuktoyaktuk",
	"Chicoutimi", "Ouiatchouane", "Chibougamau",
	"Matagami", "Kipawa", "Kinojevis",
	"Abitibi", "Maganasipi",
	/* Iceland */
	"Akureyri", "Kopasker", "Budereyri", "Akranes", "Bordeyri",
	"Holmavik",
};
#define NSHKGENERAL (sizeof shkgeneral / sizeof (char *))

void
get_names(int type, char *name, char *shop_type)
{
	switch(type) {
		case -1:
			strcpy(name, shkgeneral[rnd(NSHKGENERAL)]);
			strcpy(shop_type, "assorted antiques");
			break;
		case 0:
			strcpy(name, shkliquors[rnd(NSHKLIQUORS)]);
			strcpy(shop_type, "liquor");
			break;
		case 1:
			strcpy(name, shkbooks[rnd(NSHKBOOKS)]);
			strcpy(shop_type, "second hand book");
			break;
		case 2:
			strcpy(name, shkfoods[rnd(NSHKFOODS)]);
			strcpy(shop_type, "delicatessen");
			break;
		case 3:
			strcpy(name, shkweapons[rnd(NSHKWEAPONS)]);
			strcpy(shop_type, "antique weapon");
			break;
		case 4:
			strcpy(name, shkarmors[rnd(NSHKARMORS)]);
			strcpy(shop_type, "used armor");
			break;
		case 5:
			strcpy(name, shkrings[rnd(NSHKRINGS)]);
			strcpy(shop_type, "engagement ring");
			break;
		case 6:
			strcpy(name, shkwands[rnd(NSHKWANDS)]);
			strcpy(shop_type, "walking cane");
			break;
	}
}

void
treasure_room() {
	register int i, j;
	register struct room *rp;
	register struct linked_list *item, *exitptr;
	register struct object *cur;
	int got_unique = FALSE;
	int length, width;

	/* Count the number of free spaces */
	i = 0;	/* 0 tries */
	do {
	    rp = &rooms[rnd_room()];
	    width = rp->r_max.y - 2;
	    length = rp->r_max.x - 2;
	} until ((width*length <= MAXTREAS) || (i++ > MAXROOMS*4));

	/* Mark the room as a treasure room */
	rm_turn_on(rp, ISTREAS);

	/* Make all the doors secret doors */
	for (exitptr = rp->r_exit;exitptr != NULL; exitptr = next(exitptr)) {
	    cmov(*DOORPTR(exitptr));
	    addch(SECRETDOOR);
	}

	/* Put in the monsters and treasures */
	for (j=1; j<rp->r_max.y-1; j++)
	    for (i=1; i<rp->r_max.x-1; i++) {
		coord trp;

		trp.y = rp->r_pos.y+j;
		trp.x = rp->r_pos.x+i;

		/* Monsters */
		if ((rnd(100) < (MAXTREAS*100)/(width*length)) &&
		    (mvwinch(mw, rp->r_pos.y+j, rp->r_pos.x+i) == ' ')) {
		    register struct thing *tp;

		    /* Make a monster */
		    item = new_item(sizeof *tp);
		    tp = THINGPTR(item);

		    /* 
		     * Put it there and aggravate it (unless it can escape) 
		     * only put one UNIQUE per treasure room at most
		     */
		    if (got_unique)
			new_monster(item,randmonster(FALSE, TRUE),&trp,FALSE);
		    else {
			new_monster(item,randmonster(FALSE, FALSE),&trp,FALSE);
			if (on(*tp, ISUNIQUE))
			    got_unique = TRUE;
		    }
		    turn_on(*tp, ISMEAN);
		    if (off(*tp, CANINWALL)) {
			tp->t_dest = &hero;
			turn_on(*tp, ISRUN);
		    }

		    /* If it is a mimic, undisguise it */
		    if (on(*tp, ISDISGUISE)) turn_off(*tp, ISDISGUISE);
		}

		/* Treasures */
		if (rnd(100) < (MAXTREAS*100/(width*length)) &&
		    (mvinch(rp->r_pos.y+j, rp->r_pos.x+i) == FLOOR)) {
		    item = new_thing(-1);
		    if (item != NULL) {
		    	attach(lvl_obj, item);
		    	cur = (struct object *) ldata(item);
	
		    	mvaddch(trp.y, trp.x, cur->o_type);
		    	cur->o_pos = trp;
		    }
		}
	    }
}


void
shop_room() {
	register int i, j;
	register struct room *rp;
	register struct linked_list *item;
	register struct object *cur;
	int length, width, type, c;
	coord *exit;
	int exitx, exity;

	/* Count the number of free spaces */
	i = 0;	/* 0 tries */
	do {
            rp = &rooms[rnd_room()];
	    width = rp->r_max.y - 2;
	    length = rp->r_max.x - 2;
	    if (rm_off(rp, ISTREAS) && (width * length <= 20) && (width * length >= 10))
		break;
	} until (i++ > MAXROOMS*16);

	if (rm_on(rp, ISTREAS) || (width * length >= 20) || (width * length <= 10))
		return;

	if (next(rp->r_exit) != NULL) {
		if (next(next(rp->r_exit)) != NULL) {
			return;
		}
		else if (rnd(100) > 25) {
			return;
		}
	}

	for (c=1;c<10;c++) {
		type = rnd(8)-1;
	
		switch(type) {
			case -1: /* assorted */
				goto found_shop;
			case 0:  /* liquor */
				if (level > 10 && level < 20)
					goto found_shop;
				break;
			case 1:  /* second hand book */
				if (level > 15 && level < 25)
					goto found_shop;
				break;
			case 2:  /* deli */
				if (rnd(10) > 5)
					goto found_shop;
				break;
			case 3:  /* antique weapon */
				if (level > 5 && level < 15)
					goto found_shop;
				break;
			case 4:  /* used armor */
				if (level < 10)
					goto found_shop;
				break;
			case 5:  /* engagement ring */
				if (level > 25)
					goto found_shop;
				break;
			case 6:  /* walking cane */
				if (level > 20)
					goto found_shop;
				break;
		}
	}
	return;

found_shop:
			
	get_names(type, shkname, shtype);

	rm_turn_on(rp, ISSHOP);
	rm_turn_on(rp, ISTREAS);
	rm_turn_on(rp, HASFIRE);

	/* Put in the treasures */
	for (j=1; j<rp->r_max.y-1; j++)
	    for (i=1; i<rp->r_max.x-1; i++) {
		coord trp;

		trp.y = rp->r_pos.y+j;
		trp.x = rp->r_pos.x+i;


		/* Treasures */
		if (mvinch(trp.y, trp.x) == GOLD) {
			item = find_obj(trp.y, trp.x);
			detach(lvl_obj, item);
			mvaddch(trp.y, trp.x, FLOOR);
			o_discard(item);
		}

		if (mvinch(trp.y, trp.x) == FLOOR) {
		    item = new_thing(type);
		    if (item != NULL) {
		    	attach(lvl_obj, item);
		    	cur = (struct object *) ldata(item);
		    	mvaddch(trp.y, trp.x, cur->o_type);
		    	cur->o_pos = trp;
			cur->o_count = 1;
			cur->o_flags |= ISUNPAID;
		    }
		}
	    }


	/* Put in shopkeeper */

	exit = DOORPTR(rp->r_exit);
	exity = exit->y;
	exitx = exit->x;

	if (mvinch(exity, exitx-1) == PASSAGE) {
		shop_door.x = exitx+1;
		shop_door.y = exity;
		shk_pos.x = exitx+1;
		if (!isrock(mvinch(exity+1, exitx+1)))
			shk_pos.y = exity+1;
		else
			shk_pos.y = exity-1;
	}
	else if (mvinch(exity, exitx+1) == PASSAGE) {
		shop_door.x = exitx-1;
		shop_door.y = exity;
		shk_pos.x = exitx-1;
		if (!isrock(mvinch(exity+1, exitx-1)))
			shk_pos.y = exity+1;
		else
			shk_pos.y = exity-1;
	}
	else if (mvinch(exity-1, exitx) == PASSAGE) {
		shop_door.x = exitx;
		shop_door.y = exity+1;
		shk_pos.y = exity+1;
		if (!isrock(mvinch(exity+1, exitx+1)))
			shk_pos.x = exitx+1;
		else
			shk_pos.x = exitx-1;
	}
	else if (mvinch(exity+1, exitx) == PASSAGE) {
		shop_door.x = exitx;
		shop_door.y = exity-1;
		shk_pos.y = exity-1;
		if (!isrock(mvinch(exity-1, exitx+1)))
			shk_pos.x = exitx+1;
		else
			shk_pos.x = exitx-1;
	}

	item = new_item(sizeof *shk);
	shk = THINGPTR(item);
	new_monster(item, NUMMONST, &shk_pos, TRUE);
	turn_off(*shk, CANSELL);
	turn_on(*shk, ISSHK);
	turn_on(*shk, ISRUN);
	turn_on(*shk, ISHASTE);
}


void
special_zoo() {
	register int i, j;
	register struct room *rp;
	register struct linked_list *item, *exitptr;
	register struct object *cur;
	coord trp;
	int length, width, c, jelly_set;
	int monst_num;

	/* Count the number of free spaces */
	i = 0;	/* 0 tries */
	do {
	    rp = &rooms[rnd_room()];
	    width = rp->r_max.y - 2;
	    length = rp->r_max.x - 2;
	} until ((width*length <= MAXTREAS) || (i++ > MAXROOMS*2));

	if ((rm_on(rp, ISTREAS)) || (rm_on(rp, ISSHOP)))
		return;

	trp.y = rp->r_pos.y+1;
	trp.x = rp->r_pos.x+1;

	for (c=0;c<7;c++) {
		int i, top;

		if (level < 5) {
			top = 8 / (5-level);
		}
		else {
			top = 10;
		}
		
		monst_num = randmonster(FALSE, TRUE);
		for (i=0; i<top;i++)
			if (monsters[monst_num].m_flags[i] == INPACK) 
				goto found_monst;
	}
	return;

found_monst:

	/* Mark the room as a treasure room */
	rm_turn_on(rp, ISTREAS);
	if (strcmp("giant rat", monsters[monst_num].m_name) == 0)
		rm_turn_on(rp, ISNEST);
	if (strcmp("orc", monsters[monst_num].m_name) == 0)
		rm_turn_on(rp, ISORCDEN);
	if (strcmp("giant ant", monsters[monst_num].m_name) == 0)
		rm_turn_on(rp, ISHILL);
	if (strcmp("hobgoblin", monsters[monst_num].m_name) == 0)
		rm_turn_on(rp, ISHGDEN);
	if (strcmp("ghoul", monsters[monst_num].m_name) == 0)
		rm_turn_on(rp, ISMORGUE);
	if (strcmp("troll", monsters[monst_num].m_name) == 0)
		rm_turn_on(rp, ISTRDEN);
	if (strcmp("mummy", monsters[monst_num].m_name) == 0)
		rm_turn_on(rp, ISTOMB);
	if (strcmp("killer bee", monsters[monst_num].m_name) == 0)
		rm_turn_on(rp, ISHIVE);
	if (strcmp("vampire", monsters[monst_num].m_name) == 0)
		rm_turn_on(rp, ISVAMDEN);

	/* Make all the doors secret doors */
	for (exitptr = rp->r_exit;exitptr != NULL; exitptr = next(exitptr)) {
	    if (rnd(100) > 5) {
	    	cmov(*DOORPTR(exitptr));
	    	addch(SECRETDOOR);
	    }
	}

	jelly_set = FALSE;
	/* Put in the monsters and treasures */
	for (j=1; j<rp->r_max.y-1; j++)
	    for (i=1; i<rp->r_max.x-1; i++) {
		coord trp;

		trp.y = rp->r_pos.y+j;
		trp.x = rp->r_pos.x+i;

		if (rnd(100) > 75 && j > 3)
			continue;

		/* Monsters */
		if ((rnd(100) < (MAXTREAS*100)/(width*length)) &&
		    (mvwinch(mw, rp->r_pos.y+j, rp->r_pos.x+i) == ' ')) {
		    register struct thing *tp;

		    /* Make a monster */
		    item = new_item(sizeof *tp);
		    tp = THINGPTR(item);

		    new_monster(item,monst_num,&trp,FALSE);
		    turn_on(*tp, ISMEAN);
		    if (off(*tp, CANINWALL)) {
			tp->t_dest = &hero;
			turn_on(*tp, ISRUN);
		    }
		}
		/* Treasure */
		if (rm_on(rp, ISNEST)) {
			if (rnd(200) < (MAXTREAS*100/(width*length)) &&
		    		(mvinch(rp->r_pos.y+j, rp->r_pos.x+i) == FLOOR)) {
	    			item = new_thing(2);
	    			attach(lvl_obj, item);
	    			cur = OBJPTR(item);
	    			rnd_pos(rp, &trp);

	    			mvaddch(trp.y, trp.x, FOOD);
	    			cur->o_pos = trp;
			}
		}
		if (rm_on(rp, ISORCDEN)) {
			if (rnd(200) < (MAXTREAS*100/(width*length)) &&
		    		(mvinch(rp->r_pos.y+j, rp->r_pos.x+i) == FLOOR)) {
	    			item = spec_item(GOLD, 0, 0, 0);
	    			attach(lvl_obj, item);
	    			cur = OBJPTR(item);
	    			rnd_pos(rp, &trp);

	    			mvaddch(trp.y, trp.x, GOLD);
	    			cur->o_pos = trp;
			}
		}
		if (rm_on(rp, ISHIVE)) {
			if (rnd(200) < (MAXTREAS*100/(width*length)) &&
		    		(mvinch(rp->r_pos.y+j, rp->r_pos.x+i) == FLOOR) && !jelly_set) {
	    			item = new_thing(2);
	    			attach(lvl_obj, item);
	    			cur = OBJPTR(item);
	    			rnd_pos(rp, &trp);

	    			mvaddch(trp.y, trp.x, FOOD);
	    			cur->o_pos = trp;
	    			cur->o_which = 4;
				jelly_set = TRUE;
			}
		}
		if (rm_on(rp, ISHGDEN)) {
			if (rnd(200) < (MAXTREAS*100/(width*length)) &&
		    		(mvinch(rp->r_pos.y+j, rp->r_pos.x+i) == FLOOR)) {
	    			item = spec_item(GOLD, 0, 0, 0);
	    			attach(lvl_obj, item);
	    			cur = OBJPTR(item);
	    			rnd_pos(rp, &trp);

	    			mvaddch(trp.y, trp.x, GOLD);
	    			cur->o_pos = trp;
			}
		}
		if (rm_on(rp, ISMORGUE)) {
			if (rnd(200) < (MAXTREAS*100/(width*length)) &&
		    		(mvinch(rp->r_pos.y+j, rp->r_pos.x+i) == FLOOR)) {
		    		item = new_thing(-1);
		    		if (item != NULL) {
		    			attach(lvl_obj, item);
		    			cur = (struct object *) ldata(item);
				
		    			mvaddch(trp.y, trp.x, cur->o_type);
		    			cur->o_pos = trp;
		    		}
			}
		}
		if (rm_on(rp, ISTRDEN)) {
			if (rnd(200) < (MAXTREAS*100/(width*length)) &&
		    		(mvinch(rp->r_pos.y+j, rp->r_pos.x+i) == FLOOR)) {
		    		item = new_thing(-1);
		    		if (item != NULL) {
		    			attach(lvl_obj, item);
		    			cur = (struct object *) ldata(item);
		
		    			mvaddch(trp.y, trp.x, cur->o_type);
		    			cur->o_pos = trp;
		    		}
			}
		}
		if (rm_on(rp, ISTOMB)) {
			if (rnd(200) < (MAXTREAS*100/(width*length)) &&
		    		(mvinch(rp->r_pos.y+j, rp->r_pos.x+i) == FLOOR)) {
		    		item = new_thing(-1);
		    		if (item != NULL) {
		    			attach(lvl_obj, item);
		    			cur = (struct object *) ldata(item);
	
		    			mvaddch(trp.y, trp.x, cur->o_type);
		    			cur->o_pos = trp;
		    		}
			}
		}
		if (rm_on(rp, ISVAMDEN)) {
		}
		if (rm_on(rp, ISHILL)) {
			if (rnd(200) < (MAXTREAS*100/(width*length)) &&
		    		(mvinch(rp->r_pos.y+j, rp->r_pos.x+i) == FLOOR)) {
	    			item = new_thing(2);
	    			attach(lvl_obj, item);
	    			cur = OBJPTR(item);
	    			rnd_pos(rp, &trp);

	    			mvaddch(trp.y, trp.x, FOOD);
	    			cur->o_pos = trp;
			}
		}
	    }
}

void
throne_room() {
	register int i, j;
	register struct room *rp;
	register struct linked_list *item, *exitptr;
	register struct object *cur;
	register struct thing *tp;
	coord trp;
	int length, width, c;
	int monst_num;

	/* Count the number of free spaces */
	i = 0;	/* 0 tries */
	do {
	    rp = &rooms[rnd_room()];
	    width = rp->r_max.y - 2;
	    length = rp->r_max.x - 2;
	    if ((rm_on(rp, ISTREAS)) || (rm_on(rp, ISSHOP)))
		continue;
	} until ((width*length <= MAXTREAS) || (i++ > MAXROOMS*4));

	if (rm_on(rp, ISTREAS) || rm_on(rp, ISSHOP))
		return;

	trp.y = rp->r_pos.y+1;
	trp.x = rp->r_pos.x+1;

	for (c=0;c<40;c++) {
		int i;

		monst_num = randmonster(FALSE, FALSE);
		for (i=0; i<10;i++)
			if (monsters[monst_num].m_flags[i] == ISUNIQUE) 
				goto found_unique;
	}
	return;

found_unique:

	item = new_item(sizeof *tp);
	tp = THINGPTR(item);

	new_monster(item,monst_num,&trp,FALSE);
	turn_on(*tp, ISMEAN);
	if (off(*tp, CANINWALL)) {
		tp->t_dest = &hero;
		turn_on(*tp, ISRUN);
	}

	/* Mark the room as a treasure room */
	rm_turn_on(rp, ISTREAS);
	rm_turn_on(rp, ISTHROOM);

	/* Make all the doors secret doors */
	for (exitptr = rp->r_exit;exitptr != NULL; exitptr = next(exitptr)) {
	    if (rnd(100) > 5) {
	    	cmov(*DOORPTR(exitptr));
	    	addch(SECRETDOOR);
	    }
	}

	/* Put in the monsters and treasures */
	for (j=1; j<rp->r_max.y-1; j++)
	    for (i=1; i<rp->r_max.x-1; i++) {
		coord trp;

		trp.y = rp->r_pos.y+j;
		trp.x = rp->r_pos.x+i;

		/* Treasures */
		if (rnd(100) < (MAXTREAS*100/(width*length)) &&
		    (mvinch(rp->r_pos.y+j, rp->r_pos.x+i) == FLOOR)) {
		    item = new_thing(-1);
		    if (item != NULL) {
		    	attach(lvl_obj, item);
		    	cur = (struct object *) ldata(item);
	
		    	mvaddch(trp.y, trp.x, cur->o_type);
		    	cur->o_pos = trp;
		    }
		}
	    }
}
