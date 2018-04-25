/* bijou HTTP server for plan 9 by sean caron (scaron@umich.edu) */

#include "main.h"

/* return codes:
  *
  * 0 = success
  * 1 = http 500 internal server error
  * 2 = http 401 authorization required
*/

int check_authorization(char *path, char *rpath, struct configuration cfg, struct request req, struct htpasswd *htpass) {
    Dir *td;	
    char *u, *p, *temp;
    int q, j, r;
    uchar digest[SHA1dlen];
    DigestState *s;
    char up[41];

    temp = malloc(sizeof(char)*(strlen(path)+1));

    if (temp == 0) {
        exits("MALLOC");
    }

    strcpy(temp, path);

    if (temp[strlen(temp)-1] != '/') {
        for (j = strlen(temp); j > 0; j--) {
            if (temp[j] == '/') {
                temp[j+1] = '\0';
                break;
            }
        }
    }

    temp = realloc(temp, sizeof(char)*(strlen(temp)+strlen(cfg.htpassfile)+2));

    if (temp == 0) {
        exits("REALLOC");
    }

    strcat(temp, cfg.htpassfile);

    while (1) {
        // try the path on hand
        td = dirstat(temp);

        // didnt find htpasswd file, chop the path and keep looking
        if (td == nil) {
            for (j = strlen(temp); j >= 0; j--) {
                if (temp[j] == '/') {
                    temp[j] = '\0';
                    break;
                }
            }

            // if chopping off the lash slash leaves us with rpath then break out of the loop
            if (strcmp(temp, rpath) == 0) {
                break;
            }

            for (j = strlen(temp); j >= 0; j--) {
                if (temp[j] == '/') {
                    temp[j+1] = '\0';
                    break;
                }
            }

            strcat(temp, cfg.htpassfile);
        }

        // found a htpasswd file, break out of this htpasswd locator loop
        else {
            break;
        }
    }

    // at this point, we have in temp either equal to rpath, or a path to a htpasswd file if one exists.

    // so basically, check to see: if temp is not rpath (contains a path to a htpasswd file)
    if (strcmp(temp, rpath) != 0) {
        // then read in the htpasswd file
        r = read_htpasswd_file(temp, htpass);

        // if the function returned with an error code, give the client a HTTP 500 internal server error
        if (r != 0) {
            free(temp);
            return 1;
        }

        // if we were ok, then:
        // split req.authorization into username and password (it should already be decoded)
        // username and password are colon delimited

        r = 0;

        // maybe we should make a copy of req.authorization before strtok'ing it...
        u = strtok(req.authorization, ":");

        // this sort of error handling is really crude; this should get refactored later.
        if (u == 0) {
            free(temp);
            return 2;
        }

        p = strtok(0, ":");

        if (p == 0) {
            free(temp);
            return 2;
        }

        // compare user name and password with entries read in from htpasswd file

        r = 0;

        for (q = 0; q < htpass->n_users; q++) {
            if (strcmp( u, *(htpass->user+q)) == 0) {
                s = sha1( (uchar *)p, strlen(p), digest, nil);

                sprintf(up, "%02.x%02.x%02.x%02.x%02.x%02.x%02.x%02.x%02.x%02.x%02.x%02.x%02.x%02.x%02.x%02.x%02.x%02.x%02.x%02.x", digest[0], digest[1], digest[2], digest[3], digest[4], digest[5], digest[6], digest[7], digest[8], digest[9], digest[10], digest[11], digest[12], digest[13], digest[14], digest[15], digest[16], digest[17], digest[18], digest[19] );

                if (strcmp(up, *(htpass->pass+q)) == 0) {
                    r = 1;
                    break;
                }
            }
        }

        // if there is no match, give http 401 authorization required error
        if (r != 1) {
            free(temp);
            return 2;
        }

        // otherwise, success; just pass through, we will return 0 and serve up the page
    }

    free(temp);

    return 0;
}

