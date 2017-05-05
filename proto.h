/*   $Id: proto.h,v 1.9 2017/05/05 04:26:44 loehrj Exp $
 *
 * CS 352 -- Mini Shell!  
 *
 *   April 5, 2017,  Josh Loehr
 *
 */

#include <dirent.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

/* Constants */ 

#define LINELEN 1024
#define EOS     '\0'
#define STDOUT  1
#define STDERR  2

/* Macros */

#define POW10(X) (pows[X])
static const int pows[] = { 1, 10, 100, 1000, 10000, 100000, 1000000 };

/* Global Variables */

extern int margc;
extern char **margv;
extern int margshift;
extern int exit_status;

/* Prototypes */

char **arg_parse (char *line, int *argcp);
int expand (char *orig, char *new, int newsize);
int handle_builtins (int argc, char **argv);
void processline (char *line);

