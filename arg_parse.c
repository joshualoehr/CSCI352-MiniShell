/* CS 352 -- Mini Shell - arg_parse  
 *
 *   April 5, 2017,  Josh Loehr
 *
 */

#include "proto.h"

void remove_all(char *str, char c)
{
    char *src = str;
    char *dst = str;

    for (; *src != EOS; src++) {
        if (*src != c) {
            *(dst++) = *src;
        }      
    }
    *dst = EOS;
}

char **arg_parse (char *line, int *argcp)
{
    int argc = 0;     // argument count
    char **argv;      // argument array
    int idx = 0;
    int in_quote = 0; // flag to determine if current char is within quotes

    /* Count the number of arguments */
    while (1) {
        char c = line[idx++];
        
        if (c == EOS) {
            if (in_quote) {
                fprintf(stderr, "Error: mismatched quotes.\n");
                *argcp = 0;
                return (char **) NULL;
            }
            break;      
        } else if (c != ' ') {
            /* Step forward until idx points after the arg */
            do {
                if (c == '"') {
                    in_quote = !in_quote;      
                }
                c = line[idx++];
            } while ((in_quote || c != ' ') && c != EOS);

            argc++;
        }
    } 

    /* Store the argument count */
    *argcp = argc;

    /* Initialize argv array */
    if ((argv = (char **) malloc (sizeof(char *) * (argc + 1))) == NULL) {
        perror("malloc"); 
    }
    argv[argc] = (char *) NULL;

    /* Decrement idx back to the EOS terminating line */
    idx--;

    /* Loop back over line, assigning arg pointers and adding EOS symbols */
    in_quote = 0;
    while (argc > 0) {
        if (line[idx-1] != ' ') {
            line[idx--] = EOS;

            /* Step backwards to the beginning of the arg */
            in_quote = (line[idx] == '"');
            while (idx > 0 && (in_quote || line[idx-1] != ' ')) {
                idx--;      
                if (line[idx] == '"') {
                    in_quote = !in_quote;      
                }
            }

            remove_all(&line[idx], '"');
            argv[--argc] = &line[idx];
        }     

        idx--;
    }

    return argv;
}
