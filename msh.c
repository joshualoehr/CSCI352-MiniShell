/* CS 352 -- Mini Shell!  
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

/* Shell main */

int
main (void)
{
    char   buffer [LINELEN];
    int    len;

    while (1) {

        /* prompt and get line */
        fprintf (stderr, "%% ");
        if (fgets (buffer, LINELEN, stdin) != buffer)
          break;

            /* Get rid of \n at end of buffer. */
        len = strlen(buffer);
        if (buffer[len-1] == '\n')
            buffer[len-1] = 0;

        /* Run it ... */
        processline (buffer);

    }

    if (!feof(stdin))
        perror ("read");

    return 0;		/* Also known as exit (0); */
}


void processline (char *line)
{
    pid_t  cpid;
    int    status;

    int    argc;
    char **argv = arg_parse (line, &argc);

    if (argc == 0) {
        free(argv);
        return;      
    }

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
    if (wait (&status) < 0)
        perror ("wait");

    free(argv);
}

