/* bijou HTTP server for plan 9 by sean caron (scaron@umich.edu) */

#include "main.h"


/* Returns 0 if request version is properly formatted, returns 1 if the format is erroneous*/
int check_request_version(char *version) {
    char *temp;
    int i;

    temp = strtok(version, "/");

    if (temp == 0) {
        return 1;
    }

    if (strcmp(temp, "HTTP") != 0) {
        return 1;
    }

    temp = strtok(0, ".");

    if (temp == 0) {
        return 1;
    }

    for (i = 0; i < strlen(temp); i++) {
        if (isdigit(temp[i]) == 0) {
            return 1;
        }
    }

    temp = strtok(0, ".");

    if (temp == 0) {
        return 1;
    }

    for (i = 0; i < strlen(temp); i++) {
        if ( isdigit(temp[i]) == 0 ) {
            return 1;
        }
    }

    temp = strtok(0, " ");

    if (temp != 0) {
        return 1;
    }

    return 0;
}

