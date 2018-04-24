/* htpasswd file entry generator for bijou HTTP server by sean caron (scaron@umich.edu) */

#include <u.h>
#include <libc.h>
#include <authsrv.h>
#include <mp.h>
#include <libsec.h>
#include <stdio.h>

int main(int argc, char **argv);
int read_line(int infd, char **ptr);

int main(int argc, char **argv) {
    uchar digest[SHA1dlen];

    DigestState *s;

    if (argc != 3) {
        printf("usage: %s username password >> .htpasswd\n", argv[0]);
        exits(0);
    }

    int i;

    s = sha1( (uchar *)argv[2], strlen(argv[2]), digest, nil);

    printf("%s:", argv[1] );

    for (i = 0; i < SHA1dlen; i++) {
        printf("%02.x", digest[i] );
    }

    printf("\n");

    return 0;
}

/* read in a line from the specified file descriptor to the specified buffer */
int read_line(int infd, char **ptr) {
    int len = 1;
    int n;

    *ptr = malloc(sizeof(char));

    while (1) {
        n = read(infd, &(*(*ptr+len-1)), 1);

        *ptr = realloc(*ptr, (len+1)*sizeof(char));

        if (*ptr == 0) {
            exits("REALLOC");
        }

        if (*(*ptr+len-1) == 10) {
            *(*ptr+len) = '\0';
            break;
        }

        /* disconnected */
        if (n == 0) {
            exits("DISCONNECTED");
        }

        len = len + 1;
    }

    return len;
}
