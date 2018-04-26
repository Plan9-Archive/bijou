/* bijou HTTP server for plan 9 by sean caron (scaron@umich.edu) */

#include "main.h"

/* return codes:
 * 0 = success
 * 1 = failed to open htpasswd file
 * 2 = htpasswd parse error
 * 3 = no users specified in htpasswd file or htpasswd file empty
*/

int read_htpasswd_file(char *path, struct htpasswd *htpass) {
    int n, fd;
    char *line, *temp;

    int len = 1;

    int lr = 0;

    /* initialize some configuration variables */
    htpass->n_users = 0;

    htpass->user = malloc( sizeof(char *) );

    if (htpass->user == 0) {
        exits("MALLOC");
    }

    htpass->pass = malloc ( sizeof(char *) );

    if (htpass->pass == 0) {
        exits("MALLOC");
    }

    /* attempt to open the htpasswd file */
    fd = open(path, OREAD);

    /* if the file does not exist, return to the caller with an error code */
    if (fd < 0) {
        return 1;
    }

    /* if it exists, read it in */
    line = malloc(sizeof(char));

    while (1) {
        n = read(fd, &(*(line+len-1)), 1);

        line = realloc(line, (len+1)*sizeof(char));

        if (line == 0) {
            exits("REALLOC");
        }

        /* finished reading a line */
        if (*(line+len-1) == '\n') {
            /* terminate it */

            *(line+len-1) = '\0';

            /* exclude comments, blank lines, and lines starting with invalid characters */
            if ((line[0] != '#' ) && (strlen(line)>0)) {
                /* the first valid, non-comment line in the file is the realm */

                if (lr == 0) {
                    htpass->realm = malloc( sizeof(char) * (strlen(line) + 1) );

                    if (htpass->realm == 0) {
                        exits("MALLOC");
                    }

                    strcpy(htpass->realm, line);
                }

                /* otherwise the lines are interpreted as username:password */
                /* password is sha-1 hashed */
                else {
                    /* make some more room in the first level of the array */

                    if (htpass->n_users > 0) {
                        htpass->user = realloc(htpass->user, (sizeof(char *) * (htpass->n_users+1)) );

                        if (htpass->user == 0) {
                            exits("REALLOC");
                        }

                        htpass->pass = realloc(htpass->pass, (sizeof(char *) * (htpass->n_users+1)) );

                        if (htpass->pass == 0) {
                            exits("REALLOC");
                        }
                    }

                    /* allocate some memory for the username and password */
                    *(htpass->user+htpass->n_users) = malloc( sizeof(char) * (strlen(line) + 1) );

                    if (*(htpass->user+htpass->n_users) == 0) {
                        exits("MALLOC");
                    }

                    *(htpass->pass+htpass->n_users) = malloc( sizeof(char) * (strlen(line) + 1) );

                    if (*(htpass->pass+htpass->n_users) == 0) {
                        exits("MALLOC");
                    }

                    /* parse each line into username:password pairs */
                    temp = strtok(line, ":");

                    /* parse error */
                    if (temp == 0) {
                        free(line);
                        return 2;
                    }

                    else {
                        strcpy( *(htpass->user+htpass->n_users), temp);
                    }

                    temp = strtok(0, ":");

                    /* parse error */
                    if (temp == 0) {
                        free(line);
                        return 2;
                    }

                    else {
                        strcpy( *(htpass->pass+htpass->n_users), temp);
                    }

                    htpass->n_users = htpass->n_users + 1;
                }
            }

            /* reset the buffer for the next input line */
            free(line);

            line = malloc(sizeof(char));

            if (line == 0) {
                exits("MALLOC");
            }

            len = 1;

            lr = lr + 1;
        }

        else {
            len = len + 1;
        }

        /* eof */
        if (n == 0) {
            break;
        }
    }

    close(fd);

    free(line);

    if (htpass->n_users > 0) {
        return 0;
    }

    else {
        return 3;
    }
}

