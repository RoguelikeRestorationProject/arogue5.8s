/*
 * Rogue definitions and variable declarations
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

#include "mdport.h"

#define reg	register	/* register abbr.	*/
#define NOOP(x) (x += 0)
#define CCHAR(x) ( (char) (x & A_CHARTEXT) )

#define MY_LINES	 (LINES > 30 ? 30 : LINES)
#define MY_COLS	 	 (COLS >  80 ? 80 : COLS)

/*
 * Maximum number of different things
 */

#define MAXDAEMONS	10
#define MAXFUSES	20

#define NCOLORS         45
#define NSTONES         58
#define NWOOD           29
#define NMETAL          23
#define NSYLLS          159

#define	MAXROOMS	9
#define	MAXTHINGS	9
#define	MAXOBJ		9
#define MAXSTATS	62	/* max total of all stats at startup */
#define	MAXPACK		23
#define	MAXCONTENTS	10
#define MAXENCHANT	10	/* max number of enchantments on an item */
#define	MAXTREAS	15	/* number monsters/treasure in treasure room */
#define	MAXTRAPS	20
#define	MAXTRPTRY	8	/* attempts/level allowed for setting traps */
#define	MAXDOORS	4	/* Maximum doors to a room */
#define	MAXPRAYERS	15	/* Maximum number of prayers for cleric */
#define	MAXSPELLS	20	/* Maximum number of spells (for magician) */
#define	NUMMONST	149	/* Current number of monsters */
#define NUMUNIQUE	24	/* number of UNIQUE creatures */
#define	NLEVMONS	4	/* Number of new monsters per level */
#define	MAXFOODS	1
#define NUMSCORE	20	/* number of entries in score file */
#define HARDER		35	/* at this level start making things harder */
#define MAXPURCH	4	/* max purchases per trading post visit */
#define LINELEN		80	/* characters in a buffer */
#define JUG_EMPTY	-1	/* signifys that the alchemy jug is empty */

/* Movement penalties */
#define BACKPENALTY 3
#define SHOTPENALTY 2		/* In line of sight of missile */
#define DOORPENALTY 1		/* Moving out of current room */

/*
 * these defines are used in calls to get_item() to signify what
 * it is we want
 */
#define	ALL		-1
#define	WEARABLE	-2
#define	CALLABLE	-3
#define WIELDABLE	-4
#define USEABLE		-5
#define IDENTABLE	-6
#define REMOVABLE	-7
#define PROTECTABLE	-8
#define ZAPPABLE	-9

/*
 * stuff to do with encumberance
 */
#define NORMENCB	1500	/* normal encumberance */
#define F_OKAY		 0	/* have plenty of food in stomach */
#define F_HUNGRY	 1	/* player is hungry */
#define F_WEAK		 2	/* weak from lack of food */
#define F_FAINT		 3	/* fainting from lack of food */
#define F_SATIATED	 4      /* satiated stomach */

/*
 * return values for get functions
 */
#define	NORM	0	/* normal exit */
#define	QUIT	1	/* quit option setting */
#define	MINUS	2	/* back up one option */

/* 
 * The character types
 */
#define	C_FIGHTER	0
#define	C_MAGICIAN	1
#define	C_CLERIC	2
#define	C_THIEF		3
#define	C_MONSTER	4

/*
 * Number of hit points for going up a level
 */
#define HIT_FIGHTER	10
#define HIT_MAGICIAN	8
#define HIT_CLERIC	8
#define HIT_THIEF	6

/*
 * values for games end
 */
#define UPDATE  -2
#define SCOREIT -1
#define KILLED 	 0
#define CHICKEN  1
#define WINNER   2

/*
 * definitions for function step_ok:
 *	MONSTOK indicates it is OK to step on a monster -- it
 *	is only OK when stepping diagonally AROUND a monster
 */
#define MONSTOK 1
#define NOMONST 2

/*
 * used for ring stuff
 */
#define LEFT_1		 0
#define LEFT_2		 1
#define LEFT_3		 2
#define LEFT_4		 3
#define RIGHT_1		 4
#define RIGHT_2		 5
#define RIGHT_3		 6
#define RIGHT_4		 7
#define NUM_FINGERS	 8

/*
 * used for micellaneous magic (MM) stuff
 */
#define WEAR_BOOTS	0
#define WEAR_BRACERS	1
#define WEAR_CLOAK	2
#define WEAR_GAUNTLET	3
#define WEAR_JEWEL	4
#define WEAR_NECKLACE	5
#define NUM_MM		6

/*
 * All the fun defines
 */
#define next(ptr) (*ptr).l_next
#define prev(ptr) (*ptr).l_prev
#define ldata(ptr) (*ptr).l_data
#define inroom(rp, cp) (\
    (cp)->x <= (rp)->r_pos.x + ((rp)->r_max.x - 1) && (rp)->r_pos.x <= (cp)->x \
 && (cp)->y <= (rp)->r_pos.y + ((rp)->r_max.y - 1) && (rp)->r_pos.y <= (cp)->y)
#define winat(y, x) (mvwinch(mw, y, x)==' '?mvwinch(stdscr, y, x):winch(mw))
#define debug if (wizard) msg
#define RN (((seed = seed*11109+13849) & 0x7fff) >> 1)
#define unc(cp) (cp).y, (cp).x
#define cmov(xy) move((xy).y, (xy).x)
#define DISTANCE(y1, x1, y2, x2) ((x2 - x1)*(x2 - x1) + (y2 - y1)*(y2 - y1))
#define OBJPTR(what)	(struct object *)((*what).l_data)
#define THINGPTR(what)	(struct thing *)((*what).l_data)
#define DOORPTR(what)	(coord *)((*what).l_data)
#define when break;case
#define otherwise break;default
#define until(expr) while(!(expr))
#define ce(a, b) ((a).x == (b).x && (a).y == (b).y)
#define draw(window) wrefresh(window)
#define hero player.t_pos
#define pstats player.t_stats
#define max_stats player.maxstats
#define pack player.t_pack
#define attach(a, b) _attach(&a, b)
#define detach(a, b) _detach(&a, b)
#define o_free_list(a) _o_free_list(&a)
#define t_free_list(a) _t_free_list(&a)
#undef max /* PDCurses defines these */
#undef min /* PDCurses defines these */
#define max(a, b) ((a) > (b) ? (a) : (b))
#define min(a, b) ((a) < (b) ? (a) : (b))
#define on(thing, flag) \
    (((thing).t_flags[(flag >> FLAGSHIFT) & FLAGINDEX] & flag) != 0)
#define off(thing, flag) \
    (((thing).t_flags[(flag >> FLAGSHIFT) & FLAGINDEX] & flag) == 0)
#define turn_on(thing, flag) \
    ((thing).t_flags[(flag >> FLAGSHIFT) & FLAGINDEX] |= (flag & ~FLAGMASK))
#define turn_off(thing, flag) \
    ((thing).t_flags[(flag >> FLAGSHIFT) & FLAGINDEX] &= ~flag)

#define rm_on(rm, flag) \
    (((rm)->r_flags[(flag >> FLAGSHIFT) & FLAGINDEX] & flag) != 0)
#define rm_off(rm, flag) \
    (((rm)->r_flags[(flag >> FLAGSHIFT) & FLAGINDEX] & flag) == 0)
#define rm_turn_on(rm, flag) \
    ((rm)->r_flags[(flag >> FLAGSHIFT) & FLAGINDEX] |= (flag & ~FLAGMASK))
#define rm_turn_off(rm, flag) \
    ((rm)->r_flags[(flag >> FLAGSHIFT) & FLAGINDEX] &= ~flag)

#undef CTRL
#define CTRL(ch) (ch & 037)

#define ALLOC(x) calloc((unsigned int) x,1)
#define FREE(x) free((char *) x)
#define	EQSTR(a, b, c)	(strncmp(a, b, c) == 0)
#define	EQUAL(a, b)	(strcmp(a, b) == 0)
#define GOLDCALC (rnd(50 + 10 * level) + 2)
#define ISRING(h, r) (cur_ring[h] != NULL && cur_ring[h]->o_which == r)
#define ISWEARING(r)	(ISRING(LEFT_1, r) || ISRING(LEFT_2, r) ||\
			 ISRING(LEFT_3, r) || ISRING(LEFT_4, r) ||\
			 ISRING(RIGHT_1, r) || ISRING(RIGHT_2, r) ||\
			 ISRING(RIGHT_3, r) || ISRING(RIGHT_4, r))
#define newgrp() ++group
#define o_charges o_ac
#define ISMULT(type) (type == FOOD)
#define isrock(ch) ((ch == WALL) || (ch == '-') || (ch == '|'))
#define invisible(monst) \
	    (((on(*monst, ISINVIS) || \
	       (on(*monst, ISSHADOW) && rnd(100) < 90)) && \
	      off(player, CANSEE)) || \
	     (on(*monst, CANSURPRISE) && !ISWEARING(R_ALERT)) || \
	     (on(*monst, CANFALL) && !ISWEARING(R_ALERT)))
#define is_stealth(tp) \
    (rnd(25) < (tp)->t_stats.s_dext || (tp == &player && ISWEARING(R_STEALTH)))

#define has_light(rp) (((rp)->r_flags & HASFIRE) || ISWEARING(R_LIGHT))
#define mi_wght mi_worth

/*
 * Ways to die
 */
#define	D_PETRIFY	-1
#define	D_ARROW		-2
#define	D_DART		-3
#define	D_POISON	-4
#define	D_BOLT		-5
#define	D_SUFFOCATION	-6
#define	D_POTION	-7
#define	D_INFESTATION	-8
#define D_DROWN		-9
#define D_ROT		-10
#define D_CONSTITUTION  -11
#define D_STRENGTH	-12
#define D_SIGNAL	-13
#define D_CHOKE		-14
#define D_STRANGLE	-15
#define D_FALL		-16
#define D_RELIC		-17
#define D_FCHOKE	-18
#define D_STARVE	-19
#define DEATHNUM	19	/* number of ways to die */

/*
 * Things that appear on the screens
 */
#define	WALL		' '
#define	PASSAGE		'#'
#define	DOOR		'+'
#define	FLOOR		'.'
#define VPLAYER 	'@'
#define IPLAYER 	'_'
#define	POST		'^'
#define	TRAPDOOR	'>'
#define	ARROWTRAP	'{'
#define	SLEEPTRAP	'$'
#define	BEARTRAP	'}'
#define	TELTRAP		'~'
#define	DARTTRAP	'`'
#define POOL		'"'
#define MAZETRAP	'\''
#define	SECRETDOOR	'&'
#define	STAIRS		'%'
#define	GOLD		'*'
#define	POTION		'!'
#define	SCROLL		'?'
#define	MAGIC		'$'
#define	BMAGIC		'>'	/*	Blessed	magic	*/
#define	CMAGIC		'<'	/*	Cursed	magic	*/
#define	FOOD		':'
#define	WEAPON		')'
#define MISSILE		'*'	/*	Magic Missile	*/
#define	ARMOR		']'
#define	MM		';'
#define RELIC		','
#define	RING		'='
#define	STICK		'/'
#define FOREST		'\\'
#define HEAVY_BALL	'0'

/*
 * Various constants
 */
#define	PASSWD		"mTBellIQOsLNA"
#define	BEARTIME	3
#define PUNISHTIME      300
#define	SLEEPTIME	4
#define	FREEZETIME	6
#define PAINTIME	(roll(1, 6))
#define	HEALTIME	30
#define	CHILLTIME	(roll(20, 4))
#define	SMELLTIME	20
#define	STONETIME	8
#define HASTETIME	6
#define	SICKTIME	10
#define	STPOS		0
#define	WANDERTIME	(max(5, HARDER-rnd(vlevel)))
#define	BEFORE		1
#define	AFTER		2
#define	HUHDURATION	20
#define	SEEDURATION	850
#define	CLRDURATION	15
#define GONETIME	200
#define FLYTIME		300
#define DUSTTIME	(20+roll(1,10))
#define	PHASEDURATION	300
#define	HUNGERTIME	1300
#define NOWEAPONTIME    100
#define	MORETIME	150
#define STARVETIME	200
#define	STINKTIME	6
#define	STOMACHSIZE	2000
#define	ESCAPE		27
#define	BOLT_LENGTH	10
#define	MARKLEN		20
#define DAYLENGTH	400
#define ALCHEMYTIME	(400+rnd(100))

/*
 * Save against things
 */
#define	VS_POISON		00
#define	VS_PARALYZATION		00
#define	VS_DEATH		00
#define	VS_PETRIFICATION	01
#define	VS_WAND			02
#define	VS_BREATH		03
#define	VS_MAGIC		04

/*
 * attributes for treasures in dungeon
 */
#define ISCURSED     	       01
#define ISKNOW      	       02
#define ISPOST		       04	/* object is in a trading post */
#define	ISMETAL     	      010
#define ISPROT		      020	/* object is protected */
#define ISBLESSED     	      040
#define ISUNPAID           010000       /* Unpaid item */
#define ISMISL      	   020000
#define ISMANY     	   040000
/*
 * Various flag bits
 */
#define ISDARK	    	       01
#define ISGONE	    	       02
#define	ISTREAS     	       04
#define ISFOUND     	      010
#define ISTHIEFSET	      020
#define FORCEDARK	      040
#define ISSHOP		     0100
#define ISNEST		     0200
#define ISORCDEN	     0400
#define ISHILL		    01000
#define ISHGDEN	            02000
#define ISMORGUE	    04000
#define ISTRDEN		   010000
#define ISHIVE		   040000
#define ISVAMDEN 	  0100000
#define ISTHROOM	  0200000
#define ISTOMB		  0400000
/*
 * 1st set of creature flags (this might include player)
 */
#define ISBLIND		0x00000001
#define	ISINWALL     	0x00000002
#define ISRUN		0x00000004
#define	ISFLEE		0x00000008
#define ISINVIS		0x00000010
#define ISMEAN		0x00000020
#define ISGREED		0x00000040
#define CANSHOOT	0x00000080
#define ISHELD		0x00000100
#define ISHUH		0x00000200
#define ISREGEN		0x00000400
#define CANHUH		0x00000800
#define CANSEE		0x00001000
#define HASFIRE		0x00002000
#define ISSLOW		0x00004000
#define ISHASTE		0x00008000
#define ISCLEAR		0x00010000
#define CANINWALL	0x00020000
#define ISDISGUISE	0x00040000
#define CANBLINK	0x00080000
#define CANSNORE	0x00100000
#define HALFDAMAGE	0x00200000
#define	CANSUCK		0x00400000
#define	CANRUST		0x00800000
#define	CANPOISON	0x01000000
#define	CANDRAIN	0x02000000
#define ISUNIQUE	0x04000000
#define	STEALGOLD	0x08000000
/* 
 * Second set of flags 
 */
#define	STEALMAGIC	0x10000001
#define	CANDISEASE	0x10000002
#define HASDISEASE	0x10000004
#define CANSUFFOCATE	0x10000008
#define DIDSUFFOCATE	0x10000010
#define BOLTDIVIDE	0x10000020
#define BLOWDIVIDE	0x10000040
#define NOCOLD		0x10000080
#define	TOUCHFEAR	0x10000100
#define BMAGICHIT	0x10000200
#define NOFIRE		0x10000400
#define NOBOLT		0x10000800
#define CARRYGOLD	0x10001000
#define CANITCH		0x10002000
#define HASITCH		0x10004000
#define DIDDRAIN	0x10008000
#define WASTURNED	0x10010000
#define CANSELL		0x10020000
#define CANBLIND	0x10040000
#define NOACID		0x10080000
#define NOSLOW		0x10100000
#define NOFEAR		0x10200000
#define NOSLEEP		0x10400000
#define NOPARALYZE	0x10800000
#define NOGAS		0x11000000
#define CANMISSILE	0x12000000
#define CMAGICHIT	0x14000000
#define CANPAIN		0x18000000

/* 
 * Third set of flags 
 */
#define CANSLOW		0x20000001
#define CANTUNNEL	0x20000002
#define TAKEWISDOM	0x20000004
#define NOMETAL		0x20000008
#define MAGICHIT	0x20000010
#define CANINFEST	0x20000020
#define HASINFEST	0x20000040
#define NOMOVE		0x20000080
#define CANSHRIEK	0x20000100
#define CANDRAW		0x20000200
#define CANSMELL	0x20000400
#define CANPARALYZE	0x20000800
#define CANROT		0x20001000
#define ISSCAVENGE	0x20002000
#define DOROT		0x20004000
#define CANSTINK	0x20008000
#define HASSTINK	0x20010000
#define ISSHADOW	0x20020000
#define CANCHILL	0x20040000
#define	CANHUG		0x20080000
#define CANSURPRISE	0x20100000
#define CANFRIGHTEN	0x20200000
#define CANSUMMON	0x20400000
#define TOUCHSTONE	0x20800000
#define LOOKSTONE	0x21000000
#define CANHOLD		0x22000000
#define DIDHOLD		0x24000000
#define DOUBLEDRAIN	0x28000000

/* 
 * Fourth set of flags 
 */
#define CANBRANDOM	0x30000001	/* Types of breath */
#define CANBACID	0x30000002	/* acid */
#define CANBFIRE	0x30000004	/* Fire */
#define CANBCGAS	0x30000008	/* confusion gas */
#define CANBBOLT	0x30000010	/* lightning bolt */
#define CANBGAS		0x30000020	/* clorine gas */
#define CANBICE		0x30000040	/* ice */
#define CANBFGAS	0x30000080	/* Fear gas */
#define CANBPGAS	0x30000100	/* Paralyze gas */
#define CANBSGAS	0x30000200	/* Sleeping gas */
#define CANBSLGAS	0x30000400	/* Slow gas */
#define CANBREATHE	0x300007ff	/* Can it breathe at all? */
/*
 * Fifth set of flags
 */
#define ISUNDEAD	0x40000001
#define CANSONIC	0x40000002
#define TURNABLE	0x40000004
#define TAKEINTEL	0x40000008
#define NOSTAB		0x40000010
#define CANDISSOLVE	0x40000020
#define ISFLY		0x40000040	/* creature can fly */
#define CANTELEPORT	0x40000080	/* creature can teleport */
#define CANEXPLODE	0x40000100	/* creature explodes when hit */
#define CANDANCE	0x40000200	/* creature can make hero "dance" */
#define ISDANCE		0x40000400	/* creature (hero) is dancing */
#define CARRYFOOD	0x40000800
#define CARRYSCROLL	0x40001000
#define CARRYPOTION	0x40002000
#define CARRYRING	0x40004000
#define CARRYSTICK	0x40008000
#define CARRYMISC	0x40010000
#define CARRYDAGGER	0x40020000	/* Dagger of Musty */
#define CARRYCLOAK	0x40040000	/* Cloak of Emori */
#define CARRYANKH	0x40080000	/* Ankh of Heil */
#define CARRYSTAFF	0x40100000	/* Staff of Ming */
#define CARRYWAND	0x40200000	/* Wand of Orcus */
#define	CARRYROD	0x40400000	/* Rod of Asmodeus */
#define	CARRYAMULET	0x40800000	/* Amulet of Yendor */
#define	CARRYMANDOLIN	0x41000000	/* Mandolin of Brian */
#define MISSEDDISP	0x42000000	/* Missed Cloak of Displacement */
#define CANBSTAB	0x44000000	/* Can backstab */


#define CARRYHORN	0x50000001	/* Horn of Geryon */
#define CARRYMSTAR	0x50000002	/* Morning Star of Hruggek */
#define CARRYFLAIL	0x50000004	/* Flail of Yeenoghu */
#define CARRYWEAPON	0x50000008	/* A generic weapon */
#define CANAGE		0x50000010	/* can age you */
#define ISSHK           0x50000020      /* A shopkeeper */
#define INPACK	        0x50000040      /* Can it run in packs? */
#define NOSNORE	        0x50000100      /* Can it be put to sleep? */
#define ISFLOAT	        0x50000200      /* Can it float? */
#define CANFREEZE	0x50000400	/* Can it freeze the hero? */
#define CANFLASH	0x50001000	/* Can it blind the hero by a flash? */
#define CANFALL		0x50002000	/* Can it fall on the hero? */
#define CANBURDEN	0x50004000	/* Can it add to the burden of the hero? */
#define CANFEXPLODE	0x50010000	/* Can it explode and freeze the hero? */
#define ISREADY		0x60000001
#define ISDEAD		0x60000002
#define ISELSEWHERE	0x60000004

/* Masks for choosing the right flag */
#define FLAGMASK     0xf0000000
#define FLAGINDEX    0x0000000f
#define FLAGSHIFT    28
#define MAXFLAGS     25			/* max initial flags per creature */

/* 
 * Mask for cancelling special abilities 
 * The flags listed here will be the ones left on after the
 * cancellation takes place
 */
#define CANC0MASK (	ISBLIND		| ISINWALL	| ISRUN		| \
			ISFLEE		| ISMEAN	| ISGREED	| \
			CANSHOOT	| ISHELD	| ISHUH		| \
			ISSLOW		| ISHASTE	| ISCLEAR	| \
			ISUNIQUE )
#define CANC1MASK (	HASDISEASE	| DIDSUFFOCATE	| CARRYGOLD 	| \
			HASITCH		| CANSELL 	| DIDDRAIN	| \
			WASTURNED )
#define CANC2MASK (	HASINFEST	| NOMOVE	| ISSCAVENGE	| \
			DOROT		| HASSTINK	| DIDHOLD )
#define CANC3MASK (	CANBREATHE )
#define CANC4MASK (	ISUNDEAD	| CANSONIC	| NOSTAB	| \
			ISFLY		| CARRYFOOD	| CANEXPLODE	| \
			ISDANCE		| CARRYSCROLL	| CARRYPOTION	| \
			CARRYRING	| CARRYSTICK	| CARRYMISC	| \
			CARRYDAGGER	| CARRYCLOAK	| CARRYANKH	| \
			CARRYSTAFF	| CARRYWAND	| CARRYROD	| \
			CARRYAMULET	| CARRYMANDOLIN )
#define CANC5MASK (	CARRYHORN	| CARRYMSTAR 	| CARRYFLAIL )

/* types of things */
#define TYP_POTION	0
#define TYP_SCROLL	1
#define TYP_FOOD	2
#define TYP_WEAPON	3
#define TYP_ARMOR	4
#define TYP_RING	5
#define TYP_STICK	6
#define TYP_MM		7
#define	TYP_RELIC	8
#define	NUMTHINGS	9
/*
 * Potion types
 */
#define	P_CLEAR		0
#define	P_ABIL		1
#define	P_SEEINVIS	2
#define	P_HEALING	3
#define	P_MFIND		4
#define	P_TFIND		5
#define	P_RAISE		6
#define	P_HASTE		7
#define	P_RESTORE	8
#define	P_PHASE		9
#define P_INVIS		10
#define P_BOOZE		11
#define P_FRUITJUICE	12
#define P_PARALYSIS	13
#define P_FLY		14
#define	MAXPOTIONS	15
/*
 * Scroll types
 */
#define	S_CONFUSE	0
#define	S_MAP		1
#define	S_LIGHT		2
#define	S_HOLD		3
#define	S_SLEEP		4
#define	S_ALLENCH	5
#define	S_IDENT		6
#define	S_SCARE		7
#define	S_GFIND		8
#define	S_TELEP		9
#define	S_CREATE	10
#define	S_REMOVE	11
#define	S_PETRIFY	12
#define	S_GENOCIDE	13
#define	S_CURING	14
#define S_MAKEIT	15
#define S_PROTECT	16
#define S_NOTHING	17
#define S_AMNESIA	18
#define S_FIRE		19
#define S_PUNISHMENT	20
#define	MAXSCROLLS	21

/*
 * Weapon types
 */
#define MACE		0		/* mace */
#define SWORD		1		/* long sword */
#define BOW		2		/* short bow */
#define ARROW		3		/* arrow */
#define DAGGER		4		/* dagger */
#define ROCK		5		/* rocks */
#define TWOSWORD	6		/* two-handed sword */
#define SLING		7		/* sling */
#define DART		8		/* darts */
#define CROSSBOW	9		/* crossbow */
#define BOLT		10		/* crossbow bolt */
#define SPEAR		11		/* spear */
#define TRIDENT		12		/* trident */
#define SPETUM		13		/* spetum */
#define BARDICHE	14 		/* bardiche */
#define PIKE		15		/* pike */
#define BASWORD		16		/* bastard sword */
#define HALBERD		17		/* halberd */
#define BATTLEAXE	18		/* battle axe */
#define BOOMERANG	19 		/* boomerang */
#define CRYSKNIFE	20		/* crysknife */
#define PICKAXE		21		/* pick-axe */
#define WORMTOOTH	22		/* worm tooth */
#define FLAIL		23		/* flail */
#define MAXWEAPONS	24		/* types of weapons */
#define NONE		100		/* no weapon */

/*
 * Armor types
 */
#define	LEATHER		0
#define	RING_MAIL	1
#define	STUDDED_LEATHER	2
#define	SCALE_MAIL	3
#define	PADDED_ARMOR	4
#define	CHAIN_MAIL	5
#define	SPLINT_MAIL	6
#define	BANDED_MAIL	7
#define	PLATE_MAIL	8
#define	PLATE_ARMOR	9
#define SHIELD		10
#define GLOVES		11
#define HELMET		12
#define	MAXARMORS	13

/*
 * Ring types
 */
#define	R_PROTECT	0
#define	R_ADDSTR	1
#define	R_SUSABILITY	2
#define	R_SEARCH	3
#define	R_SEEINVIS	4
#define	R_ALERT		5
#define	R_AGGR		6
#define	R_ADDHIT	7
#define	R_ADDDAM	8
#define	R_REGEN		9
#define	R_DIGEST	10
#define	R_TELEPORT	11
#define	R_STEALTH	12
#define	R_ADDINTEL	13
#define	R_ADDWISDOM	14
#define	R_HEALTH	15
#define R_HEAVY		16
#define R_LIGHT		17
#define R_DELUSION	18
#define R_FEAR		19
#define R_HEROISM	20
#define R_FIRE		21
#define R_WARMTH	22
#define R_VAMPREGEN	23
#define R_NOSTONE	24
#define R_FLOAT		25
#define	MAXRINGS	26

/*
 * Rod/Wand/Staff types
 */

#define	WS_LIGHT	0
#define	WS_HIT		1
#define	WS_ELECT	2
#define	WS_FIRE		3
#define	WS_COLD		4
#define	WS_POLYMORPH	5
#define	WS_MISSILE	6
#define	WS_SLOW_M	7
#define	WS_DRAIN	8
#define	WS_CHARGE	9
#define	WS_TELMON	10
#define	WS_CANCEL	11
#define WS_CONFMON	12
#define WS_DISINTEGRATE	13
#define WS_PETRIFY	14
#define WS_PARALYZE	15
#define WS_MDEG		16
#define WS_CURING	17
#define WS_WONDER	18
#define WS_FEAR		19
#define WS_MAKEINVIS	20
#define WS_DEATH	21
#define WS_SLEEP	22
#define WS_AFFECT	23
#define	MAXSTICKS	24

/*
 * miscellaneous magic items
 */
#define	MM_JUG		0
#define	MM_BEAKER	1
#define	MM_BOOK		2
#define	MM_ELF_BOOTS	3
#define MM_BRACERS	4
#define MM_OPEN		5
#define MM_HUNGER	6
#define MM_DISP		7
#define MM_PROTECT	8
#define MM_DRUMS	9
#define MM_DISAPPEAR	10
#define MM_CHOKE	11
#define MM_G_DEXTERITY	12
#define MM_G_OGRE	13
#define MM_JEWEL	14
#define MM_KEOGHTOM	15
#define MM_R_POWERLESS	16
#define MM_FUMBLE	17
#define MM_ADAPTION	18
#define MM_STRANGLE	19
#define MM_DANCE	20
#define MM_SKILLS	21
#define MAXMM		22

/*
 * Relic types
 */
#define MUSTY_DAGGER	0
#define EMORI_CLOAK	1
#define HEIL_ANKH	2
#define MING_STAFF	3
#define ORCUS_WAND	4
#define ASMO_ROD	5
#define YENDOR_AMULET	6
#define BRIAN_MANDOLIN	7
#define GERYON_HORN	8
#define HRUGGEK_MSTAR	9
#define YEENOGHU_FLAIL	10
#define MAXRELIC	11


#define LEVEL  600
#define vlevel max(level, turns/LEVEL + 1)
/*
 * Now we define the structures and types
 */

struct delayed_action {
	int d_type;
	void (*d_func)();
	void *d_arg;
	int d_time;
};

/*
 * level types
 */
typedef enum {
	NORMLEV,	/* normal level */
	POSTLEV,	/* trading post level */
	MAZELEV,	/* maze level */
	OUTSIDE		/* outside level */
} LEVTYPE;

/*
 * Help list
 */

struct h_list {
    char h_ch;
    char *h_desc;
};

/*
 * Coordinate data type
 */
typedef struct {
    int x;
    int y;
} coord;

/*
 * structure for the ways to die
 */
struct death_type {
    int reason;
    char *name;
};


/*
 * Linked list data type
 */
struct linked_list {
    struct linked_list *l_next;
    struct linked_list *l_prev;
    char *l_data;			/* Various structure pointers */
};

/*
 * Stuff about magic items
 */

struct magic_item {
    char *mi_name;
    int mi_prob;
    int mi_worth;
    int mi_curse;
    int mi_bless;
};

/*
 * Room structure
 */
struct room {
    coord r_pos;			/* Upper left corner */
    coord r_max;			/* Size of room */
    unsigned int r_flags[4];			/* Info about the room */
    struct linked_list *r_fires;	/* List of fire creatures in room */
    struct linked_list *r_exit;		/* Linked list of exits */
};

/*
 * Array of all traps on this level
 */

struct trap {
    int tr_type;			/* What kind of trap */
    char tr_show;			/* Where disguised trap looks like */
    coord tr_pos;			/* Where trap is */
    unsigned int tr_flags;	/* Info about trap (i.e. ISFOUND) */
};

/*
 * Structure describing a fighting being
 */
struct stats {
    int s_str;			/* Strength */
    int s_intel;			/* Intelligence */
    int s_wisdom;			/* Wisdom */
    int s_dext;			/* Dexterity */
    int s_const;			/* Constitution */
    int s_charisma;			/* Charisma */
    unsigned int s_exp;		/* Experience */
    int s_lvl;				/* Level of mastery */
    int s_arm;				/* Armor class */
    int s_hpt;				/* Hit points */
    int s_pack;				/* current weight of his pack */
    int s_carry;			/* max weight he can carry */
    char s_dmg[30];			/* String describing damage done */
};

/*
 * Structure describing a fighting being (monster at initialization)
 */
struct mstats {
    short s_str;			/* Strength */
    unsigned int s_exp;		/* Experience */
    int s_lvl;				/* Level of mastery */
    int s_arm;				/* Armor class */
    char *s_hpt;			/* Hit points */
    char *s_dmg;			/* String describing damage done */
};

/*
 * Structure for monsters and player
 */
struct thing {
    bool t_turn;			/* If slowed, is it a turn to move */
    bool t_wasshot;			/* Was character shot last round? */
    char t_type;			/* What it is */
    int t_disguise;			/* What mimic looks like */
    int t_oldch;			/* Character that was where it was */
    int  t_ctype;			/* Character type */
    int t_index;			/* Index into monster table */
    short t_no_move;			/* How long the thing can't move */
    short t_quiet;			/* used in healing */
    coord *t_doorgoal;			/* What door are we heading to? */
    coord t_pos;			/* Position */
    coord t_oldpos;			/* Last position */
    coord *t_dest;			/* Where it is running to */
    unsigned int t_flags[16];		/* State word */
    struct linked_list *t_pack;		/* What the thing is carrying */
    struct stats t_stats;		/* Physical description */
    struct stats maxstats;		/* maximum(or initial) stats */
    int    t_reserved;
    int    t_reserved2;
    int    t_reserved3;
};

/*
 * Array containing information on all the various types of monsters
 */
struct monster {
    char *m_name;			/* What to call the monster */
    short m_carry;			/* Probability of carrying something */
    bool m_normal;			/* Does monster exist? */
    bool m_wander;			/* Does monster wander? */
    char m_appear;			/* What does monster look like? */
    char *m_intel;			/* Intelligence range */
    unsigned int m_flags[MAXFLAGS];		/* Things about the monster */
    char *m_typesum;			/* type of creature can he summon */
    short m_numsum;			/* how many creatures can he summon */
    short m_add_exp;			/* Added experience per hit point */
    struct mstats m_stats;		/* Initial stats */
};

/*
 * Structure for a thing that the rogue can carry
 */

struct object {
    int o_type;				/* What kind of object it is */
    coord o_pos;			/* Where it lives on the screen */
    char *o_text;			/* What it says if you read it */
    char o_launch;			/* What you need to launch it */
    char o_damage[8];			/* Damage if used like sword */
    char o_hurldmg[8];			/* Damage if thrown */
    struct linked_list *contents;	/* contents of this object */
    int o_count;			/* Count for plural objects */
    int o_which;			/* Which object of a type it is */
    int o_hplus;			/* Plusses to hit */
    int o_dplus;			/* Plusses to damage */
    int o_ac;				/* Armor class */
    unsigned int o_flags;	/* Information about objects */
    int o_group;			/* Group number for this object */
    int o_weight;			/* weight of this object */
    char o_mark[MARKLEN];		/* Mark the specific object */
};
/*
 * weapon structure
 */
struct init_weps {
    char *w_name;		/* name of weapon */
    char *w_dam;		/* hit damage */
    char *w_hrl;		/* hurl damage */
    char w_launch;		/* need to launch it */
    int  w_flags;		/* flags */
    int  w_wght;		/* weight of weapon */
    int  w_worth;		/* worth of this weapon */
};

/*
 * armor structure 
 */
struct init_armor {
	char *a_name;		/* name of armor */
	int  a_prob;		/* chance of getting armor */
	int  a_class;		/* normal armor class */
	int  a_worth;		/* worth of armor */
	int  a_wght;		/* weight of armor */
};

struct matrix {
    int base;			/* Base to-hit value (AC 10) */
    int max_lvl;		/* Maximum level for changing value */
    int factor;			/* Amount base changes each time */
    int offset;			/* What to offset level */
    int range;			/* Range of levels for each offset */
};

struct spells {
    short s_which;		/* which scroll or potion */
    short s_cost;		/* cost of casting spell */
    short s_type;		/* scroll or potion */
    int   s_flag;		/* is the spell blessed/cursed? */
};

struct linked_list *get_hurl(struct thing *tp); /* chase.c */
void do_chase(struct thing *th, int flee);
int chase(struct thing *tp, coord *ee, int flee, int *mdead);
int straight_shot(int ery, int erx, int eey, int eex, coord *shooting);
coord *can_shoot(coord *er, coord *ee);
int can_blink(struct thing *tp);
void runto(struct thing *runner, coord *spot);

void quit(int sig); /* command.c */
void help(void);
void identify(void);
void d_level(void);
void u_level(void);
void shell(void);
void call(int mark);
void bugkill(int sig);
void search(int is_thief, int door_chime);
void command(void);

void kill_daemon(void (*func)()); /* daemon.c */
void start_daemon(void (*func)(), void *arg, int type);
void extinguish(void (*func)());
void do_daemons(int flag);
void do_fuses(int flag);
void activity(void);
void fuse(void (*func)(), void *arg, int time, int type);
void lengthen(void (*func)(), int xtime);
struct delayed_action *find_slot(void (*func)());

void doctor(struct thing *tp); /* daemons.c */
void swander(void);
void runners(void);
void rollwand(void);
void trap_look(void);
void unconfuse(void);
void starvation(void);
void unpunish(void);
void noholdweapon(void);
void unsee(void);
void unstink(void);
void unclrhead(void);
void unphase(void);
void land(void);
void hangover(void);
void res_strength(void);
void sight(void);
void spinach_str(void);
void nohaste(void);
void noslow(void);
void suffocate(void);
void stomach(void);
void cure_disease(void);
void un_itch(void);
void appear(void);
void dust_appear(void);
void unchoke(void);
void alchemy(struct object *obj);
void undance(void);
void strangle(void);
void fumble(void);
void ring_search(void);
void ring_teleport(void);

void updpack(int getmax); /* encumb.c */
int packweight(void);
int itemweight(struct object *wh);
int playenc(void);
int totalenc(void);
void wghtchk(void);
int hitweight(void);

int fight(coord *mp, struct object *weap, int thrown); /* fight.c */
int attack(struct thing *mp, struct object *weapon, int thrown);
struct object *wield_weap(struct object *thrown, struct thing *mp);
int swing(int class, int at_lvl, int op_arm, int wplus);
int roll_em(struct thing *att_er, struct thing *def_er, struct object *weap, int hurl, struct object *cur_weapon, int back_stab);
char *prname(char *who, int upper);
void miss(struct object *weapon, struct thing *tp, char *er, char *ee);
void hit(struct object *weapon, struct thing *tp, char *er, char *ee, int back_stab);
int dext_plus(int dexterity);
int dext_prot(int dexterity);
int str_plus(int str);
int add_dam(int str);
int hung_dam(void);
void thunk(struct object *weap, struct thing *tp, char *mname);
void m_thunk(struct object *weap, struct thing *tp, char *mname);
void bounce(struct object *weap, struct thing *tp, char *mname);
void m_bounce(struct object *weap, struct thing *tp, char *mname);
int is_magic(struct object *obj);
void killed(struct linked_list *item, int pr, int points);
void explode(struct thing *tp);

void badcheck(char *name, struct magic_item *magic, int bound); /* init.c */
void init_colors(void);
void init_materials(void);
void init_misc(void);
void init_names(void);
void init_player(void);
void init_stones(void);
void init_things(void);

int readchar(void); /* io.c */
void doadd(char *fmt, va_list ap);
int dex_compute(void);
void status(int display);
void wait_for(WINDOW *win, int ch);
int step_ok(int y, int x, int can_on_monst, struct thing *flgptr);
void msg(char *fmt, ...);
void addmsg(char *fmt, ...);
void endmsg(void);
void ministat(void);
void show_win(WINDOW *scr, char *message);
void dbotline(WINDOW *scr, char *message);
void restscr(WINDOW *scr);
int shoot_ok(int ch);

struct linked_list *new_item(int size); /* list.c */
struct linked_list *creat_item(void);
char *new(size_t size);
void _detach(struct linked_list **list, struct linked_list *item);
void _attach(struct linked_list **list, struct linked_list *item);
void _o_free_list(struct linked_list **ptr);
void o_discard(struct linked_list *item);
void _t_free_list(struct linked_list **ptr);
void t_discard(struct linked_list *item);
void destroy_item(struct linked_list *item);

void fatal(char *s); /* main.c */
int rnd(int range);
int roll(int number, int sides);
void setup(void);
void playit(void);
int author(void);
int holiday(void);
void endit(int sig);
void tstp(int sig);

void crankout(void); /* maze.c */
void do_maze(void);
void draw_maze(void);
int findcells(int y, int x);
char *foffset(int y, int x);
int maze_view(int y, int x);
char *moffset(int y, int x);
void rmwall(int newy, int newx, int oldy, int oldx);

int m_use_item(struct thing *monster, coord *monst_pos, coord *defend_pos); /* misc.c */
void put_contents(struct object *bag, struct linked_list *item);
void take_contents(struct object *bag, struct linked_list *item);
void do_bag(struct linked_list *item);
void do_panic(void);
char *misc_name(struct object *obj);
void use_emori(void);
void use_mm(int which);

void check_residue(struct thing *tp); /* monsters.c */
int creat_mons(struct thing *person, int monster, int report);
void genmonsters(int least, int treas);
int id_monst(int monster);
void new_monster(struct linked_list *item, int type, coord *cp, int max_monster);
int randmonster(int wander, int no_unique);
void sell(struct thing *tp);
struct linked_list *wake_monster(int y, int x);
void wanderer(void);

coord *rndmove(struct thing *who); /* move.c */
struct trap *trap_at(int y, int x);
int be_trapped(struct thing *th, coord *tc);
int blue_light(int blessed, int cursed);
void corr_move(int dy, int dx);
void dip_it();
void do_move(int dy, int dx);
void do_run(int ch);
int getdelta(int match, int *dy, int *dx);
int isatrap(int ch);
void light(coord *cp);
int show(int y, int x);
int lit_room(struct room *rp);
void set_trap(struct thing *tp, int y, int x);

void new_level(LEVTYPE ltype); /* new_level.c */
int rnd_room(void);
void put_things(LEVTYPE ltype);

int get_abil(void *vp, WINDOW *win); /* options.c */
int get_quest(void *vp, WINDOW *win);
int get_ro(WINDOW *win, int oy, int ox);
int get_bool(void *vp, WINDOW *win);
int get_str(void *vp, WINDOW *win);
void put_bool(void *b, WINDOW *win);
void put_str(void *str, WINDOW *win);
void put_abil(void *ability, WINDOW *win);
void put_quest(void *quest, WINDOW *win);
void parse_opts(char *str);
void option(void);

void do_terrain(int basey, int basex, int deltay, int deltax, int fresh); /* outside.c */
int get_terrain(int one, int two, int three, int four);
int rnd_terrain(void);
void lake_check(coord *place);
void init_terrain(void);

struct linked_list *get_item(struct linked_list *list, char *purpose, int type); /* pack.c */
int add_pack(struct linked_list *item, int silent);
int grab(int y, int x);
int inventory(struct linked_list *list, int type);
void pick_up(int ch);
int pack_char(struct linked_list *list, struct object *obj);
void cur_null(struct object *op);
void idenpack(void);
int is_type(struct object *obj, int type);
void del_pack(struct linked_list *item);
void carry_obj(struct thing *mp, int chance);
void show_floor(void);
void picky_inven(void);

void do_passages(void); /* passages.c */
void conn(int r1, int r2);
void door(struct room *rm, coord *cp);

void affect(void); /* player.c */
void cast(void);
int const_bonus(void);
void gsense(void);
void pray(void);
void steal(void);

void add_const(int cursed); /* potions.c */
void add_dexterity(int cursed);
void add_haste(int blessed);
void add_intelligence(int cursed);
void add_slow(void);
void add_strength(int cursed);
void lower_level(int who);
void add_wisdom(int cursed);
void quaff(int which, int flag, int is_potion);
void res_dexterity(int howmuch);
void res_intelligence(void);
void res_wisdom(void);

int ring_value(int type); /* rings.c */
int ring_eat(int hand);
void ring_on(struct object *obj);
char *ring_num(struct object *obj);

void byebye(int sig); /* rip.c */
void death(int monst);
char *killname(int monst);
void score(unsigned int amount, int flags, int monst);
void showpack(char *howso);
void total_winner(void);

struct room *roomin(coord *cp); /* rooms.c */
void do_rooms(void);
void draw_room(struct room *rp);
void horiz(int cnt);
void rnd_pos(struct room *rp, coord *cp);
void vert(int cnt);

void auto_save(int sig); /* save.c */
int save_file(FILE *savef);
int restore(char *file, char **envp);
int encwrite(char *start, size_t size, FILE *outf);
size_t encread(char *start, size_t size, FILE *inf);
int save_game(void);

void genocide(void); /* scrolls.c */
void read_scroll(int which, int flag, int is_scroll);

void get_names(int type, char *name, char *shop_type); /* shops.c */
void treasure_room(void);
void shop_room(void);
void special_zoo(void);
void throne_room(void);

int rs_save_file(FILE *savef); /* state.c */
int rs_restore_file(FILE *inf);

int do_zap(int gotdir, int which, int flag); /* sticks.c */
void drain(int ymin, int ymax, int xmin, int xmax);
void fix_stick(struct object *cur);
int shoot_bolt(struct thing *shooter, coord start, coord dir, int get_points, int reason, char *name, int damage);

char *weap_name(struct object *obj); /* things.c */
struct linked_list *spec_item(int type, int which, int hit, int damage);
char *inv_name(struct object *obj, int drop);
struct linked_list *new_thing(int thing_type);
char *charge_str(struct object *obj);
int drop(struct linked_list *item);
int dropcheck(struct object *op);
int pick_one(struct magic_item *magic, int nitems);
char *blesscurse(int flags);
int extras(void);

void buy_it(void); /* trader.c */
int get_worth(struct object *obj);
void price_it(void);
char *typ_name(struct object *obj);

struct linked_list *find_mons(int y, int x); /* util.c */
int str_compute();
struct linked_list *find_obj(int y, int x);
coord *fallpos(coord *pos, int be_clear, int range);
int cansee(int y, int x);
int is_current(struct object *obj);
int secretdoor(int y, int x);
void aggravate(void);
unsigned int check_level(int get_spells);
void chg_str(int amt);
void raise_level(int get_spells);
void strucpy(char *s1, char *s2, size_t len);
int diag_ok(coord *sp, coord *ep, struct thing *flgptr);
void eat(void);
char *tr_name(int ch);
char *vowelstr(char *str);
int get_dir(void);
void waste_time(void);
void look(int wakeup, int runend);
int save(int which, struct thing *who, int adj);
int ac_compute(void);

char *num(int n1, int n2); /* weapons.c */
void do_motion(struct object *obj, int ydelta, int xdelta, struct thing *tp);
void fall(struct linked_list *item, int pr);
int hit_monster(int y, int x, struct object *obj, struct thing *tp);
void init_weapon(struct object *weap, int type);
void missile(int ydelta, int xdelta, struct linked_list *item, struct thing *tp);
void wield(void);

void take_off(void); /* wear.c */
void wear(void);

int makemonster(int create); /* wizard.c */
int getdeath(void);
void create_obj(int prompt, int which_item, int which_type);
int passwd(void);
void whatis(struct linked_list *what);
int teleport(void);
int getbless(void);

unsigned int netread();

#ifdef CHECKTIME
int checkout();
#endif

/*
 * Now all the global variables
 */
extern struct trap traps[];
extern struct h_list helpstr[];
extern struct h_list wiz_help[];
extern struct room rooms[];		/* One for each room -- A level */
extern struct room *oldrp;		/* Roomin(&oldpos) */
extern struct linked_list *mlist;	/* List of monsters on the level */
extern struct linked_list *tlist;	/* list of monsters fallen down traps */
extern struct death_type deaths[];	/* all the ways to die */
extern struct thing player;		/* The rogue */
extern struct thing *shk;		/* The rogue */
extern struct monster monsters[];	/* The initial monster states */
extern struct linked_list *lvl_obj;	/* List of objects on this level */
extern struct linked_list *monst_dead;	/* Indicates monster that got killed */
extern struct object *cur_weapon;	/* Which weapon he is weilding */
extern struct object *cur_armor;	/* What a well dresssed rogue wears */
extern struct object *cur_ring[];	/* Which rings are being worn */
extern struct object *cur_misc[];	/* which MM's are in use */
extern struct object *cur_gloves;
extern struct object *cur_helmet;
extern struct object *cur_shield;
extern struct magic_item things[];	/* Chances for each type of item */
extern struct magic_item s_magic[];	/* Names and chances for scrolls */
extern struct magic_item p_magic[];	/* Names and chances for potions */
extern struct magic_item r_magic[];	/* Names and chances for rings */
extern struct magic_item ws_magic[];	/* Names and chances for sticks */
extern struct magic_item m_magic[];	/* Names and chances for MM */
extern struct magic_item rel_magic[];	/* Names and chances for relics */
extern struct spells magic_spells[];	/* spells for magic users */
extern struct spells cleric_spells[];	/* spells for magic users */
extern char *cnames[][11];		/* Character level names */
extern char curpurch[];			/* name of item ready to buy */
extern char PLAYER;			/* what the player looks like */
extern int nfloors;			/* Number of floors in this dungeon */
extern int punished;
extern int mon_know[];
extern int punish_dext;
extern int punish_str;
extern int spinach_strength;
extern int noholdweap;
extern int char_type;			/* what type of character is player */
extern int foodlev;			/* how fast he eats food */
extern int level;			/* What level rogue is on */
extern int trader;			/* number of purchases */
extern int curprice;			/* price of an item */
extern int purse;			/* How much gold the rogue has */
extern size_t mpos;			/* Where cursor is on top line */
extern int ntraps;			/* Number of traps on this level */
extern int no_move;			/* Number of turns held in place */
extern int minus_enc;
extern int not_seen;
extern char shkname[32];
extern char shtype[32];
extern bool in_shop;
extern bool disturbed;
extern bool first_time;
extern bool not_paid;
extern coord shop_door;
extern coord shk_pos;
extern coord heavy_ball;
extern int no_command;			/* Number of turns asleep */
extern int inpack;			/* Number of things in pack */
extern int total;			/* Total dynamic memory bytes */
extern int lastscore;			/* Score before this turn */
extern int no_food;			/* Number of levels without food */
extern int foods_this_level;		/* num of foods this level */
extern int seed;			/* Random number seed */
extern int count;			/* Number of times to repeat command */
extern int dnum;			/* Dungeon number */
extern int max_level;			/* Deepest player has gone */
extern int cur_max;			/* Deepest player has gone currently */
extern int food_left;			/* Amount of food in hero's stomach */
extern int group;			/* Current group number */
extern int hungry_state;		/* How hungry is he */
extern int infest_dam;			/* Damage from parasites */
extern int lost_str;			/* Amount of strength lost */
extern int lost_dext;			/* amount of dexterity lost */
extern int hold_count;			/* Number of monsters holding player */
extern int trap_tries;			/* Number of attempts to set traps */
extern int pray_time;			/* Number of prayer points/exp level */
extern int spell_power;			/* Spell power left at this level */
extern int turns;			/* Number of turns player has taken */
extern int quest_item;			/* Item hero is looking for */
extern int cur_relic[];			/* Current relics */
extern int take;			/* Thing the rogue is taking */
extern char prbuf[];			/* Buffer for sprintfs */
extern char outbuf[];			/* Output buffer for stdout */
extern int runch;			/* Direction player is running */
extern char *s_names[];			/* Names of the scrolls */
extern char *p_colors[];		/* Colors of the potions */
extern char *r_stones[];		/* Stone settings of the rings */
extern struct init_weps weaps[];	/* weapons and attributes */
extern struct init_armor armors[];	/* armors and attributes */
extern char *ws_made[];			/* What sticks are made of */
extern char *release;			/* Release number of rogue */
extern char whoami[];			/* Name of player */
extern char fruit[];			/* Favorite fruit */
extern char huh[LINELEN];		/* The last message printed */
extern char *s_guess[];			/* Players guess at what scroll is */
extern char *p_guess[];			/* Players guess at what potion is */
extern char *r_guess[];			/* Players guess at what ring is */
extern char *ws_guess[];		/* Players guess at what wand is */
extern char *m_guess[];			/* Players guess at what MM is */
extern char *ws_type[];			/* Is it a wand or a staff */
extern char file_name[];		/* Save file name */
extern char score_file[];		/* Score file name */
extern char home[];			/* User's home directory */
extern WINDOW *cw;			/* Window that the player sees */
extern WINDOW *hw;			/* Used for the help command */
extern WINDOW *mw;			/* Used to store mosnters */
extern WINDOW *msgw;			/* Used to display messages */
extern bool pool_teleport;		/* just teleported from a pool */
extern bool inwhgt;			/* true if from wghtchk() */
extern bool running;			/* True if player is running */
extern bool playing;			/* True until he quits */
extern bool wizard;			/* True if allows wizard commands */
extern bool after;			/* True if we want after daemons */
extern bool notify;			/* True if player wants to know */
extern int fight_flush;		/* True if toilet input */
extern int terse;			/* True if we should be short */
extern int auto_pickup;		/* pick up things automatically? */
extern bool door_stop;			/* Stop running when we pass a door */
extern int jump;			/* Show running as series of jumps */
extern int slow_invent;		/* Inventory one line at a time */
extern bool firstmove;			/* First move after setting door_stop */
extern bool waswizard;			/* Was a wizard sometime */
extern int askme;			/* Ask about unidentified things */
extern bool s_know[];			/* Does he know what a scroll does */
extern bool p_know[];			/* Does he know what a potion does */
extern bool r_know[];			/* Does he know what a ring does */
extern bool ws_know[];			/* Does he know what a stick does */
extern bool m_know[];			/* Does he know what a MM does */
extern bool in_shell;			/* True if executing a shell */
extern bool daytime;			/* Indicates whether it is daytime */
extern coord oldpos;			/* Position before last look() call */
extern coord delta;			/* Change indicated to get_dir() */
extern coord grid[];			/* used for random pos generation */
extern char *nothing;			/* "nothing happens" msg */
extern char *spacemsg;
extern char *morestr;
extern char *retstr;
extern LEVTYPE levtype;
extern int demoncnt;
extern int fusecnt;
extern int between;
extern struct delayed_action d_list[MAXDAEMONS];
extern struct delayed_action f_list[MAXFUSES];
extern char *rainbow[NCOLORS];
extern char *sylls[NSYLLS];
extern char *stones[NSTONES];
extern char *metal[NMETAL];
extern char *wood[NWOOD];
extern coord ch_ret;
