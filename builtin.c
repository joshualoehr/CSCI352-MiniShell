/*   $Id: builtin.c,v 1.7 2017/05/03 23:40:08 loehrj Exp $
 *
 * CS 352 -- Mini Shell!  
 *
 *   April 13, 2017
 *
 */

#include "proto.h"

#define NUM_BUILTINS 8
#define OVERWRITE 1


/* PROTOTYPES */

typedef int (*funcptr) (int, char **);

typedef struct
{
    char *name;
    funcptr func;
} Command;


/* BUILTIN COMMANDS */

/* Builtin for exit [value] */
int builtin_exit_func (int argc, char **argv)
{
    int val = 0;
    if (argc > 1) {
        val = atoi(argv[1]); 
    }
    exit(val);      
    return 0;
}
Command builtin_exit = { .name="exit", .func=builtin_exit_func };

/* Builtin for aecho [-n] [arguments] */
int builtin_aecho_func (int argc, char **argv)
{
    if (argc == 1) {
        dprintf(STDOUT, "\n");      
        return 0;
    }

    int idx = 1;
    int newline_terminate = 1;

    if (argc > idx && strcmp("-n", argv[idx]) == 0) {
        newline_terminate = 0;
        idx++;

        if (argc == 2) {
            return 0;      
        }
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

    return 0;
}
Command builtin_aecho = { .name="aecho", .func=builtin_aecho_func };

/* Builtin for envset NAME value */
int builtin_envset_func (int argc, char **argv)
{
    if (argc != 3) {
        dprintf(STDERR, "Usage: envset NAME value\n");
        return 1;
    }      

    char *name = argv[1];
    char *value = argv[2];

    if (setenv(name, value, OVERWRITE) == -1) {
        perror("setenv");      
        return 1;
    }

    return 0;
}
Command builtin_envset = { .name="envset", .func=builtin_envset_func };

/* Builtin for envunset NAME */
int builtin_envunset_func (int argc, char **argv)
{
    if (argc != 2) {
        dprintf(STDERR, "Usage: envunset NAME\n");
        return 1;
    }      

    char *name = argv[1];

    if (unsetenv(name) == -1) {
        perror("unsetenv");      
        return 1;
    }

    return 0;
}
Command builtin_envunset = { .name="envunset", .func=builtin_envunset_func };

/* Builtin for cd [dir] */
int builtin_cd_func (int argc, char **argv)
{
    if (argc > 2) {
        dprintf(STDERR, "Usage: cd [dir]\n");
        return 1;
    }      

    char *target = (argc == 2) ? argv[1] : getenv("HOME");
    if (target == NULL) {
        dprintf(STDERR, "cd: Environment variable HOME not set.\n");      
        return 1;
    }

    if (chdir(target) == -1) {
        perror("cd");      
        return 1;
    }
    
    return 0;
}
Command builtin_cd = { .name="cd", .func=builtin_cd_func };

/* Builtin for shift [n] */
int builtin_shift_func (int argc, char **argv)
{
    int shift_amt;
    if (argc == 1) {
        shift_amt = 1;
    } else {
        char *endptr, *str;

        str = argv[1];
        shift_amt = strtol(str, &endptr, 10);

        if (endptr == str) {
            dprintf(STDERR, "usage: shift [n]\n");     
            return 1;
        }
    }

    if (margc == 1 || margc - 1 - margshift - shift_amt <= 0) { 
        dprintf(STDERR, "shift: not enough parameters for shift of %d\n", shift_amt);
        return 1;
    } else {
        margshift += shift_amt;
    }

    return 0;
}
Command builtin_shift = { .name="shift", .func=builtin_shift_func };

/* Builtin for unshift [n] */
int builtin_unshift_func (int argc, char **argv)
{
    int shift_amt;
    if (argc == 1) {
        margshift = 0;
        return 0;
    } else {
        char *endptr, *str;

        str = argv[1];
        shift_amt = strtol(str, &endptr, 10);

        if (endptr == str) {
            dprintf(STDERR, "usage: unshift [n]\n");     
            return 1;
        }
    }

    if (shift_amt > margshift) {
        dprintf(STDERR, "unshift: not enough parameters for unshift of %d\n", shift_amt);      
        return 1;
    } else {
        margshift -= shift_amt;
    }

    return 0;
}
Command builtin_unshift = { .name="unshift", .func=builtin_unshift_func };

/* Builtin for sstat file [file ...] */
int builtin_sstat_func (int argc, char **argv)
{
    if (argc == 1) {
        dprintf(STDERR, "usage: sstat file [file ...]\n");      
    }
}
Command builtin_sstat = { .name="sstat", .func=builtin_sstat_func };


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
    builtins[2] = builtin_envset;
    builtins[3] = builtin_envunset;
    builtins[4] = builtin_cd;
    builtins[5] = builtin_shift;
    builtins[6] = builtin_unshift;
    builtins[7] = builtin_sstat;

    /* Check for a matching builtin command */
    int i;
    for (i = 0; i < NUM_BUILTINS; i++) {
        Command cmd = builtins[i];
        if (strcmp(cmd.name, argv[0]) == 0) {

            /* Execute the builtin */
            exit_status = cmd.func(argc, argv); 

            free(builtins);
            return 1;
        }      
    }

    free(builtins);
    return 0;        
}
