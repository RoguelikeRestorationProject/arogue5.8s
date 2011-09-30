#ifdef HAVE_CONFIG_H
#ifdef PDCURSES
#undef HAVE_UNISTD_H
#undef HAVE_LIMITS_H
#undef HAVE_MEMORY_H
#undef HAVE_STRING_H
#endif
#include "config.h"
#elif defined(__DJGPP__)
#define HAVE_SYS_TYPES_H 1
#define HAVE_PROCESS_H 1
#define HAVE_PWD_H 1
/* #define HAVE_UNISTD_H 1 */
#define HAVE_TERMIOS_H 1
#define HAVE_SETGID 1
#define HAVE_GETGID 1
#define HAVE_SETUID 1
#define HAVE_GETUID 1
#define HAVE_GETPASS 1
#define HAVE_SPAWNL 1
#define HAVE_ALARM 1
#elif defined(_WIN32)
#define HAVE_CURSES_H
#define HAVE_TERM_H
#define HAVE__SPAWNL
#define HAVE_SYS_TYPES_H
#define HAVE_PROCESS_H
#define HAVE_VSNPRINTF 1
#define HAVE_IO_H 1
#elif defined(__CYGWIN__)
#define HAVE_VSNPRINTF 1
#define HAVE_SYS_TYPES_H 1
#define HAVE_PWD_H 1
#define HAVE_PWD_H 1
#define HAVE_SYS_UTSNAME_H 1
#define HAVE_ARPA_INET_H 1
#define HAVE_UNISTD_H 1
#define HAVE_TERMIOS_H 1
#define HAVE_NCURSES_TERM_H 1
#define HAVE_ESCDELAY
#define HAVE_SETGID 1
#define HAVE_GETGID 1
#define HAVE_SETUID 1
#define HAVE_GETUID 1
#define HAVE_GETPASS 1
#define HAVE_GETPWUID 1
#define HAVE_WORKING_FORK 1
#define HAVE_ALARM 1
#define HAVE_SPAWNL 1
#define HAVE__SPAWNL 1
#else /* POSIX */
#define HAVE_VSNPRINTF 1
#define HAVE_SYS_TYPES_H 1
#undef HAVE_PROCESS_H
#define HAVE_PWD_H 1
#define HAVE_PWD_H 1
#define HAVE_SYS_UTSNAME_H 1
#define HAVE_ARPA_INET_H 1
#define HAVE_UNISTD_H 1
#define HAVE_TERMIOS_H 1
#define HAVE_TERM_H 1
#undef HAVE_NCURSES_TERM_H
#undef HAVE_ESCDELAY
#define HAVE_SETGID 1
#define HAVE_GETGID 1
#define HAVE_SETUID 1
#define HAVE_GETUID 1
#define HAVE_SETREUID 1
#define HAVE_SETREGID 1
#undef HAVE_SETRESUID
#undef HAVE_SETRESGID
#define HAVE_GETPASS 1
#define HAVE_GETPWUID 1
#define HAVE_WORKING_FORK 1
#undef HAVE_SPAWNL
#undef HAVE__SPAWNL
#undef HAVE_NLIST_H
#undef HAVE_NLIST
#undef HAVE_LOADAV
#ifndef _AIX
#define HAVE_GETLOADAVG 1
#endif
#define HAVE_ALARM 1
#endif

#ifdef __DJGPP__
#undef HAVE_GETPWUID /* DJGPP's limited version doesn't even work as documented */
#endif

int  md_chmod(char *filename, int mode);
char *md_crypt(char *key, char *salt);
int  md_dsuspchar();
int  md_erasechar();
char *md_gethomedir();
char *md_getusername();
int  md_getuid();
char *md_getpass(char *prompt);
int  md_getpid();
char *md_getrealname(int uid);
void md_init();
int  md_killchar();
void md_normaluser();
void md_raw_standout();
void md_raw_standend();
int  md_readchar();
int  md_setdsuspchar(int c);
int  md_shellescape();
void md_sleep(int s);
int  md_suspchar();
int  md_hasclreol();
int  md_unlink(char *file);
int  md_unlink_open_file(char *file, FILE *inf);
void md_tstpsignal();
void md_tstphold();
void md_tstpresume();
void md_ignoreallsignals();
void md_onsignal_autosave();
void md_onsignal_exit();
void md_onsignal_default();
int  md_issymlink(char *sp);
unsigned int md_htonl(unsigned int);
unsigned int md_ntohl(unsigned int);
unsigned short md_htons(unsigned short);
unsigned short md_ntohs(unsigned short);
int md_shellescape(void);
unsigned long md_memused(void);
int md_rand(void);
void md_srand(int seed);
extern char *md_getusername();
extern char *md_gethomedir();
extern void md_flushinp();
extern char *md_getshell();
extern char *md_gethostname();
extern void md_dobinaryio();
extern char *md_getpass();
extern char *md_crypt();
extern char *md_getroguedir();
extern void md_init();
extern unsigned short md_htons(unsigned short);
extern unsigned short md_ntohs(unsigned short);
extern unsigned int md_htonl(unsigned int);
extern unsigned int md_ntohl(unsigned int);
void md_tstpresume(void);
void md_tstpsignal(void);
void md_start_checkout_timer(int time);
void md_stop_checkout_timer(void);
