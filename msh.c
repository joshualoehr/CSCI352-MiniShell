/*   $Id: msh.c,v 1.7 2017/05/03 23:40:08 loehrj Exp $
 *
 * CS 352 -- Mini Shell!  
 *
 *   Sept 21, 2000,  Phil Nelson
 *   Modified April 8, 2001 
 *   Modified January 6, 2003
 *
 *   Modified April 1, 2017,  Josh Loehr
 *   Modified April 5, 2017
 *
 */

#include "proto.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

/* Globals */
int margc;
char **margv;
int margshift = 0;
int exit_status = 0;

/* Shell main */

int
main (int argc, char **argv)
{
    margc = argc;
    margv = argv;

    char   buffer [LINELEN];
    int    len;

    FILE *in = stdin;
    if (margc > 1) {
        if ((in = fopen(margv[1], "r")) == NULL) {
            perror("fopen"); 
            exit(127);
        } 
    }

    while (1) {

        /* prompt (if interactive) and get line */
        if (in == stdin)
            fprintf (stderr, "%% ");
        if (fgets (buffer, LINELEN, in) != buffer)
            break;

        /* Get rid of \n at end of buffer. */
        len = strlen(buffer);
        if (buffer[len-1] == '\n')
            buffer[len-1] = 0;

        /* Run it ... */
        processline (buffer);

    }

    if (!feof(in))
        perror ("read");

    return 0;		/* Also known as exit (0); */
}


void processline (char *line)
{
    pid_t  cpid;

    char expanded_line[LINELEN];
    if (expand(line, expanded_line, LINELEN)) {
        return;      
    }

    int    argc;
    char **argv = arg_parse (expanded_line, &argc);

    if (argc == 0) {
        free(argv);
        return;      
    }

    /* If command is a builtin, execute it and return */
    if (handle_builtins(argc, argv)) {
        free(argv);
        return;      
    }

    /* Start a new process to do the job. */
    cpid = fork();
    if (cpid < 0) {
        perror ("fork");
        free(argv);
        return;
    }
    
    /* Check for who we are! */
    if (cpid == 0) {
        /* We are the child! */
        execvp (argv[0], argv);
        perror ("exec");
        exit (127);
    }
    
    /* Have the parent wait for child to complete */
    if (wait (&exit_status) < 0)
        perror ("wait");

    free(argv);
}

