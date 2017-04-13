/* CS 352 -- Mini Shell - builtin.c
 *
 *   April 13, 2017
 *
 */

#include "proto.h"

#define NUM_BUILTINS 2


/* PROTOTYPES */

typedef void (*funcptr) (int, char **);

typedef struct
{
    char *name;
    funcptr func;
} Command;


/* BUILTIN COMMANDS */

void builtin_exit_func (int argc, char **argv)
{
    int val = 0;
    if (argc > 1) {
        val = atoi(argv[1]); 
    }
    exit(val);      
}
Command builtin_exit = { .name="exit", .func=builtin_exit_func };


void builtin_aecho_func (int argc, char **argv)
{
    int idx = 1;
    char terminate = '\n';

    if (argc > idx && strcmp(argv[idx], "-n") == 0) {
        terminate = EOS;      
        idx++;
    }

    char frmt[4];
    strncpy("%s\0", frmt, 3);
    do {
        dprintf(1, frmt, argv[idx++]);
        strncpy(" %s", frmt, 3);
    } while (argc > idx);
    dprintf(1, "%c", terminate);
}
Command builtin_aecho = { .name="aecho", .func=builtin_aecho_func };


/* BUILTIN HANDLER */

int handle_builtins (int argc, char **argv) 
{
    Command *builtins = (Command *) malloc(sizeof(Command) * NUM_BUILTINS);
    if (builtins == NULL) {
        perror("malloc");      
    }
    builtins[0] = builtin_exit;
    builtins[1] = builtin_aecho;

    int i;
    for (i = 0; i < NUM_BUILTINS; i++) {
        Command cmd = builtins[i];
        if (strcmp(cmd.name, argv[0]) == 0) {
            cmd.func(argc, argv); 
            free(builtins);
            return 1;
        }      
    }

    free(builtins);
    return 0;        
}
