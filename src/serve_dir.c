/* bijou HTTP server for plan 9 by sean caron (scaron@umich.edu) */

#include "main.h"

/* Serve a directory (fd = file descriptor of network connection) */
int serve_dir(char *path, char *uri, char *hostname, char *port, char *icondir, Dir *dir, int fd, struct request req, struct configuration cfg) {
    int rfd;
    double fsize;
    long c, ndirs;
    char *filetyp, *ppath, displayname[32], date[32], fsizeunit;

    char *tp, *tn;

    /* try to open the directory and fail gracefully if it doesnt exist */
    rfd = open(path, OREAD);

    /* return 1 to signal the caller to give the client a 404 */
    if (rfd < 0) {
        return 1;
    }

    ndirs = dirreadall(rfd, &dir);

    close(rfd);

    filetyp = malloc( (ndirs * sizeof(char)) );

    if (filetyp == 0) {
        exits("MALLOC");
    }

    /* classify each entry as either a file or a directory */
    for (c = 0; c < ndirs; c++) {
        /* dir */
        if ((dir[c].mode & 0x80000000) == 0x80000000) {
            filetyp[c] = '1';
        }

        /* file */
        else {
            filetyp[c] = '0';
        }
    }

    rfc1123_time(date, time(0));

    /* print the HTTP 200 header */
    fprint(fd, "HTTP/1.1 200 OK\n");
    fprint(fd, "Date: %s\n", date);
    fprint(fd, "Server: bijou/1.0 (Plan 9)\n");
    fprint(fd, "Content-Type: text/html\n");

    fprint(fd, "\r\n");

    /* if the method was HEAD, return after printing the document header */
    if (strcmp(req.method, "HEAD") == 0) {
        return 0;
    }

    fprint(fd, "<HTML><HEAD>\n");
    fprint(fd, "<TITLE>Index of %s</TITLE>\n", uri );
    fprint(fd, "</HEAD><BODY>\n" );
    fprint(fd, "<H1>Index of %s</H1>\n", uri);
    fprint(fd, "<PRE><IMG SRC=\"%s/blank.gif\" ALT=\" \"> Name                     Last modified       Size  Description\n", icondir);
    fprint(fd, "<HR>\n");

    /* generate the link to the previous directory in the path */
    ppath = malloc(sizeof(char)*(strlen(uri)+1));

    if (ppath == 0) {
        exits("MALLOC");
    }

    strcpy(ppath, uri);

    if (strcmp(ppath, "/") != 0) {
        for (c = 2; c < strlen(ppath); c++) {
            if ( ppath[strlen(ppath)-c-1] == '/' ) {
                ppath[strlen(ppath)-c] = '\0';
                break;
            }
        }
    }

    uri_encode(ppath, &tp);

    fprint(fd, "<IMG SRC=\"%s/back.gif\" ALT=\"[DIR]\"> <A HREF=\"%s\">Parent Directory</A>         -                      -\n", icondir, tp);

    free(ppath);
    free(tp);

    /* give the directory listing to the client, line by line */
    for (c = 0; c < ndirs; c++) {
        file_time(date, dir[c].mtime);

        /* truncate display names if they are very long to preserve table formatting */
        if (strlen(dir[c].name) > 23) {
            sprintf(displayname, "%.20s..>", dir[c].name);
        }

        else {
            sprintf(displayname, "%s", dir[c].name);
        }

        uri_encode(uri, &tp);
        uri_encode(dir[c].name, &tn);

        /* directory */
        if (filetyp[c] == '1') {
            fprint(fd, "<IMG SRC=\"%s/dir.gif\" ALT=\"[DIR]\"> <A HREF=\"%s%s\">%s/</A>%.*s%s      -\n", icondir, tp, tn, displayname, (int)(24-strlen(displayname)), TABLE_PAD, date);
        }

        /* file */
        else {
            // do this for all files except htpasswd files, which we will suppress in the directory listing

            if (strcmp(dir[c].name, cfg.htpassfile) != 0) {
                /* determine the appropriate display unit of size for the file in question */

                /* kilobytes */
                fsize = dir[c].length/1000;
                fsizeunit = 'k';

                /* megabytes */
                if (fsize > 1000) {
                    fsize = fsize / 1000;
                    fsizeunit = 'M';
                }

                /* gigabytes */
                if (fsize > 1000) {
                    fsize = fsize / 1000;
                    fsizeunit = 'G';
                }

                /* determine whether the fractional part of the file size is greater than or equal to 0.5 units */
                if ((double)(fsize - (int)fsize) >= 0.5) {
                    /* if so print the file size down to a precision of a tenth of a unit */
                    fprint(fd, "<IMG SRC=\"%s/unknown.gif\" ALT=\"[FILE]\"> <A HREF=\"%s%s\">%s</A>%.*s%s %5.1f%c\n", icondir, tp, tn, displayname, (int)(25-strlen(displayname)), TABLE_PAD, date, fsize, fsizeunit);
                }

                else {
                    /* otherwise print the file size as the nearest whole unit */
                    fprint(fd, "<IMG SRC=\"%s/unknown.gif\" ALT=\"[FILE]\"> <A HREF=\"%s%s\">%s</A>%.*s%s %5.0f%c\n", icondir, tp, tn, displayname, (int)(25-strlen(displayname)), TABLE_PAD, date, fsize, fsizeunit);
                }
            }

            free(tp);
            free(tn);
        }
    }

    fprint(fd, "</PRE><HR>\n");
    fprint(fd, "<ADDRESS>bijou/1.0 server at %s port %s</ADDRESS>\n", hostname, port);
    fprint(fd, "</BODY></HTML>\n");
    fprint(fd, "\r\n");

    free(filetyp);

    return 0;
}

