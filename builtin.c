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

/* Builtin for exit [value] */
void builtin_exit_func (int argc, char **argv)
{
    int val = 0;
    if (argc > 1) {
        val = atoi(argv[1]); 
    }
    exit(val);      
}
Command builtin_exit = { .name="exit", .func=builtin_exit_func };

/* Builtin for aecho [-n] [arguments] */
void builtin_aecho_func (int argc, char **argv)
{
    if (argc == 1) {
        dprintf(STDOUT, "\n");      
        return;
    }

    int idx = 1;
    int newline_terminate = 1;

    if (argc > idx && strcmp("-n", argv[idx]) == 0) {
        newline_terminate = 0;
        idx++;
    }

    char frmt[3];
    strcpy(frmt, "%s");
    do {
        dprintf(STDOUT, frmt, argv[idx++]);
        strcpy(frmt, " %s");
    } while (argc > idx);

    if (newline_terminate) {
        dprintf(STDOUT, "\n");
    }
}
Command builtin_aecho = { .name="aecho", .func=builtin_aecho_func };


/* BUILTIN HANDLER */

/* Check if argv[0] is a builtin command. If it is, execute it. 
 * Returns 1 if a builtin was executed; 0 otherwise. */
int handle_builtins (int argc, char **argv) 
{
    /* Initialize Command struct array */
    Command *builtins = (Command *) malloc(sizeof(Command) * NUM_BUILTINS);
    if (builtins == NULL) {
        perror("malloc");      
    }
    builtins[0] = builtin_exit;
    builtins[1] = builtin_aecho;

    /* Check for a matching builtin command */
    int i;
    for (i = 0; i < NUM_BUILTINS; i++) {
        Command cmd = builtins[i];
        if (strcmp(cmd.name, argv[0]) == 0) {

            /* Execute the builtin */
            cmd.func(argc, argv); 

            free(builtins);
            return 1;
        }      
    }

    free(builtins);
    return 0;        
}
