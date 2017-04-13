/* CS 352 -- Mini Shell - proto.h  
 *
 *   April 5, 2017,  Josh Loehr
 *
 */

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

/* Prototypes */

char **arg_parse (char *line, int *argcp);
int handle_builtins (int argc, char **argv);
void processline (char *line);

