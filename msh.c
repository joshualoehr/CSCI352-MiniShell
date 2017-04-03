/* CS 352 -- Mini Shell!  
 *
 *   Sept 21, 2000,  Phil Nelson
 *   Modified April 8, 2001 
 *   Modified January 6, 2003
 *
 *   Modified April 1, 2017,  Josh Loehr
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

char **arg_parse (char *line);
void processline (char *line);

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

    char **argv = arg_parse (line);

    /* Start a new process to do the job. */
    cpid = fork();
    if (cpid < 0) {
      perror ("fork");
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


char **arg_parse (char *line)
{
    int argc = 0;    // argument count
    char **argv;     // argument array
    int idx = 0;

    /* Count the number of arguments */
    while (1) {
        char c = line[idx++];
        
        if (c == EOS) {
            break;      
        } else if (c != ' ') {
            argc++;
            
            /* Step forward until idx points after the arg */
            do {
                c = line[idx++];
            } while (c != ' ' && c != EOS);

            /* Step back one so idx points at last char of arg */
            idx--;
        }
    } 

    /* Initialize arg array */
    argv = (char **) malloc (sizeof(char *) * (argc + 1));
    argv[argc] = (char *) NULL;

    /* Loop back over line, assigning arg pointers and adding EOS symbols */
    while (argc > 0) {
        if (line[idx-1] != ' ') {
            line[idx--] = EOS;

            while (idx > 0 && line[idx-1] != ' ') {
                idx--;      
            }

            argv[--argc] = &line[idx];
        }     

        idx--;
    }

    return argv;
}
