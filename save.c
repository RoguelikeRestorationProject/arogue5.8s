/*
 * save and restore routines
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
#include <fcntl.h>
#include <time.h>
#include <string.h>
#include "curses.h"
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include "rogue.h"

#define ENCREAD encread
#define ENCWRITE encwrite

typedef struct stat STAT;

	extern char version[], encstr[];
/* extern bool _endwin; */
#include <errno.h>


STAT sbuf;

int
save_game()
{
    register FILE *savef;
    register int c;
    char buf[LINELEN];

    /*
     * get file name
     */
    mpos = 0;
    if (file_name[0] != '\0')
    {
	msg("Save file (%s)? ", file_name);
	do
	{
	    c = readchar();
		if (c == ESCAPE) { msg(""); return(0); }
	} while (c != 'n' && c != 'N' && c != 'y' && c != 'Y');
	mpos = 0;
	if (c == 'y' || c == 'Y')
	{
	    msg("File name: %s", file_name);
	    goto gotfile;
	}
    }

    do
    {
	msg("File name: ");
	mpos = 0;
	buf[0] = '\0';
	if (get_str(buf, msgw) == QUIT)
	{
	    msg("");
	    return FALSE;
	}
	strcpy(file_name, buf);
gotfile:
	if ((savef = fopen(file_name, "w")) == NULL)
	    msg(strerror(errno));	/* fake perror() */
    } while (savef == NULL);

    /*
     * write out encrpyted file (after a stat)
     * The fwrite is to force allocation of the buffer before the write
     */
    if (save_file(savef) == FALSE) {
	msg("Cannot create save file.");
	md_unlink(file_name);
	return(FALSE);
    }
    else return(TRUE);
}

/*
 * automatically save a file.  This is used if a HUP signal is
 * recieved
 */
void
auto_save(int sig)
{
    register FILE *savef;
    register int i;

    NOOP(sig);
    for (i = 0; i < NSIG; i++)
	signal(i, SIG_IGN);
    if (file_name[0] != '\0'	&& 
	pstats.s_hpt > 0	&&
	(savef = fopen(file_name, "w")) != NULL)
	save_file(savef);
    exit(1);
}

/*
 * write the saved game on the file
 */
int
save_file(FILE *savef)
{
    int ret;

    wmove(cw, MY_LINES-1, 0);
    draw(cw);
    ENCWRITE(version,strlen(version)+1,savef);
    sprintf(prbuf,"%d x %d\n", MY_LINES, MY_COLS);
    ENCWRITE(prbuf,80,savef);
    ret = rs_save_file(savef);
    fclose(savef);
    return(!ret);
}

int
restore(char *file, char **envp)
{
    FILE *inf;
    extern char **environ;
    char buf[LINELEN];
    STAT sbuf2;
    int oldcol, oldline;	/* Old number of columns and lines */

    if (strcmp(file, "-r") == 0)
	file = file_name;
    if ((inf = fopen(file, "r")) == NULL)
    {
	perror(file);
	return FALSE;
    }

    fflush(stdout);
    ENCREAD(buf, strlen(version) + 1, inf);
    if (strcmp(buf, version) != 0)
    {
	printf("Sorry, saved game is out of date.\n");
	return FALSE;
    }

    /*
     * Get the lines and columns from the previous game
     */

    ENCREAD(buf, 80, inf);
    sscanf(buf, "%d x %d\n", &oldline, &oldcol);
    stat(file, &sbuf2);
    fflush(stdout);

    /*
     * Set the new terminal and make sure we aren't going to a smaller screen.
     */

    initscr();

    if (MY_COLS < oldcol || MY_LINES < oldline) {
	printf("Cannot restart the game on a smaller screen.\n");
	return FALSE;
    }

    cw = newwin(MY_LINES, MY_COLS, 0, 0);
    mw = newwin(MY_LINES, MY_COLS, 0, 0);
    hw = newwin(MY_LINES, MY_COLS, 0, 0);
    msgw = newwin(4, MY_COLS, 0, 0);
    keypad(cw,1);
    keypad(msgw,1);

    mpos = 0;
    mvwprintw(cw, 0, 0, "%s: %s", file, ctime(&sbuf2.st_mtime));

    /*
     * defeat multiple restarting from the same place
     */
    if (!wizard) {
	if (sbuf2.st_nlink != 1) {
	    printf("Cannot restore from a linked file\n");
	    return FALSE;
	}
    }

    if (rs_restore_file(inf) != 0)
    {
        endwin();
        printf("\nCannot restore file\n");
        return(FALSE);
    }

    if (!wizard)
    {
        if (md_unlink(file) < 0) {
            fclose(inf); /* only close if system insists */
            if (md_unlink(file) < 0) {
                endwin();
                printf("\nCannot unlink file\n");
                return FALSE;
            }
        }
    }

    environ = envp;
    strcpy(file_name, file);
    setup();
    clearok(curscr, TRUE);
    touchwin(cw);
    srand(md_getpid());
    playit();
    return(FALSE);
    /*NOTREACHED*/
}

/*
 * perform an encrypted write
 */
int
encwrite(char *start, size_t size, FILE *outf)
{
    register char *ep;
    register int num_written = 0;

    ep = encstr;

    while (size--)
    {
	if (putc(*start++ ^ *ep++, outf) == EOF && ferror(outf))
	    return(num_written);
	num_written++;
	if (*ep == '\0')
	    ep = encstr;
    }
    return(num_written);
}

/*
 * perform an encrypted read
 */
size_t
encread(char *start, size_t size, FILE *inf)
{
    register char *ep;
    size_t read_size;

    if ((read_size = fread(start,1,size,inf)) == 0 || read_size == -1)
	return read_size;

    ep = encstr;

    size = read_size;
    while (size--)
    {
	*start++ ^= *ep++;
	if (*ep == '\0')
	    ep = encstr;
    }
    return read_size;
}
