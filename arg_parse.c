/* CS 352 -- Mini Shell - arg_parse  
 *
 *   April 5, 2017,  Josh Loehr
 *
 */

#include "proto.h"

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
            break;      
        } else if (c == '"') {
            in_quote = !in_quote;     
        } else if (c != ' ') {
            if (!in_quote) {
                argc++;
            }
            
            /* Step forward until idx points after the arg */
            do {
                c = line[idx++];
                if (c == '"') {
                    in_quote = !in_quote;      
                }
            } while (in_quote || (c != ' ' && c != EOS));

            /* Step back one so idx points at last char of arg */
            idx--;
        }
    } 

    /* Store the argument count */
    *argcp = argc;

    /* Initialize argv array */
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
