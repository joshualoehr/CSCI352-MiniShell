/* CS 352 -- Mini Shell - arg_parse  
 *
 *   April 5, 2017,  Josh Loehr
 *
 */

#include "proto.h"

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
