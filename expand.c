/*   $Id: expand.c,v 1.10 2017/05/05 04:26:44 loehrj Exp $
 *
 * CS 352 -- Mini Shell!
 *
 *   April 23, 2017,  Josh Loehr
 *
 */

#include "proto.h"

#define SUCCESS 0
#define FAILURE -1

int expand (char *orig, char *new, int newsize) {
    int orig_len = strlen(orig);
    int orig_idx = 0;
    int new_idx = 0;

    while (orig_idx < orig_len) {
        char c = orig[orig_idx++];

        // Wildcard support
        if (c == '*') {
            char prev = orig[orig_idx-2];

            if (prev == '\\') {
                new[new_idx - 1] = c;
            } else if (prev == ' ' || prev == '"') {
                c = orig[orig_idx++];

                char *context = NULL;

                if (c != ' ' && c != EOS && c != '"') {
                    context = &orig[orig_idx-1];
                    while (orig_idx <= orig_len && (c = orig[orig_idx++]) != ' ' 
                           && c != EOS && c != '"' && c != '/');      
                    
                    if (c == '/') {
                        dprintf(STDERR, "%s: no match\n", margv[0]);      
                        return FAILURE;
                    }

                    orig[orig_idx-1] = EOS;
                }

                // Simple case - replace with all non-hidden files
                if (c == ' ' || c == EOS || c == '"') {
                    DIR *curr_dir;
                    if ((curr_dir = opendir(".")) == NULL) {
                        perror("opendir");      
                        return FAILURE;
                    }

                    struct dirent *entry;
                    errno = 0; // To distinguish success/failure after call 
                    int first_entry = 1; // flag for spacing
                    int at_least_one_match = 0;

                    while ((entry = readdir(curr_dir)) != NULL) {
                        if (entry->d_name[0] != '.') {

                            int matches_context = 1;
                            if (context != NULL) {
                                int c_len = strlen(context);
                                int e_len = strlen(entry->d_name);
                                char *entry_name_end = &entry->d_name[e_len - c_len];

                                if (strcmp(context, entry_name_end) != 0) {
                                    matches_context = 0;      
                                }
                            }

                            if (matches_context) {
                                at_least_one_match = 1;

                                if (first_entry) {
                                    first_entry = 0;
                                } else {
                                    new[new_idx++] = ' ';      
                                }

                                int r_idx = 0;
                                while (entry->d_name[r_idx] != EOS) {
                                    new[new_idx++] = entry->d_name[r_idx++]; 
                                }       
                            }
                        }
                    }

                    if (errno != 0) {
                        perror("readdir");      
                        return FAILURE;
                    }

                    if (closedir(curr_dir) < 0) {
                        perror("closedir");
                        return FAILURE;      
                    }

                    if (context != NULL && !at_least_one_match) {
                        new[new_idx++] = '*';

                        int r_idx = 0;
                        while (context[r_idx] != EOS) {
                            new[new_idx++] = context[r_idx++];
                        }     
                    }
                }      
                new[new_idx++] = c;      
            } else {
                new[new_idx++] = c;                         
            }
                  
        } else if (c == '$') {
            c = orig[orig_idx++];      

            if (c == '{') {
                char *replace_orig = &orig[orig_idx];
                char *replace_new;

                /* Step forward until a matching brace is found */
                while (orig_idx < orig_len && (c = orig[orig_idx++]) != '}');      

                if (c != '}') {
                    dprintf(STDERR, "msh: no matching }\n");
                    return FAILURE;      
                }

                /* Temporarily put EOS char in place of closing brace */
                orig[--orig_idx] = EOS;               

                /* Insert the env var (if it exists) into new */
                if ((replace_new = getenv(replace_orig)) != NULL) { 
                    int r_len = strlen(replace_new);
                    if (new_idx + r_len > newsize) {
                        dprintf(STDERR, "msh: expansion too long\n");      
                        return FAILURE;
                    }

                    int r_idx = 0;
                    while (replace_new[r_idx] != EOS) {
                        new[new_idx++] = replace_new[r_idx++]; 
                    }
                }

                /* Replace the closing brace in orig */
                orig[orig_idx++] = '}';
            } else if (c == '$') {
                /* Get PID and convert to string */
                int pid = getpid();
                char str_pid[10];
                snprintf(str_pid, 10, "%d", pid);
                
                /* Place PID string in new string */
                int p_idx = 0;
                while (str_pid[p_idx] != EOS) {
                    new[new_idx++] = str_pid[p_idx++];      
                }

            } else if (isdigit(c)) {
                char *replace_orig = &orig[--orig_idx];
                char *replace_new = "";

                while (orig_idx <= orig_len && isdigit(c = orig[orig_idx++]));     

                orig[--orig_idx] = EOS;

                int argi = atoi(replace_orig);
                if (argi == 0) {
                    replace_new = (margc == 1) ? margv[0] : margv[1]; 
                } else if (margc > argi + 1 + margshift) {
                    replace_new = margv[argi + 1 + margshift];      
                }

                int r_len = strlen(replace_new);
                if (new_idx + r_len > newsize) {
                    dprintf(STDERR, "msh: expansion too long\n");      
                    return FAILURE;
                }

                int r_idx = 0;
                while (replace_new[r_idx] != EOS) {
                    new[new_idx++] = replace_new[r_idx++]; 
                }

                orig[orig_idx] = c;

            } else if (c == '#') {
                int argc = (margc > 1) ? margc - 1 : margc;
                argc -= margshift;

                int num_digits = 0;
                while (argc / (int) POW10(++num_digits) > 0);

                char str_argc[num_digits+1];
                snprintf(str_argc, num_digits+1, "%d", argc);

                int r_idx = 0;
                while (str_argc[r_idx] != EOS) {
                    new[new_idx++] = str_argc[r_idx++];      
                }

            } else if (c == '?') {
                int status = WEXITSTATUS(exit_status);

                int num_digits = 0;
                while (status / (int) POW10(++num_digits) > 0);

                char str_status[num_digits+1];
                snprintf(str_status, num_digits+1, "%d", status);

                int r_idx = 0;
                while (str_status[r_idx] != EOS) {
                    new[new_idx++] = str_status[r_idx++];      
                }

            } else {
                new[new_idx++] = '$';
                new[new_idx++] = c;
            }
        } else { 
            new[new_idx++] = c;
        }
    }
    new[new_idx] = EOS;
    
    return SUCCESS;    
}
