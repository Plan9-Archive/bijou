/* bijou HTTP server for plan 9 by sean caron (scaron@umich.edu) */

#include "main.h"

/* serve a file (fd=file descriptor of network connection) */

int serve_file(char *path, Dir *dir, int fd, struct request req) {

	char date[32];
	char mdate[32];
	char fbuf[8192];		// default fossil block size

	int w, sfd;

	/* try to open the file and fail gracefully if it doesnt exist */

	sfd = open(path, OREAD);

	// if opening file fails, return 1 to signal caller to give 404 and exit

	if ( sfd < 0 ) {
		return 1;
	}

	/* print HTTP 200 header */

	rfc1123_time(date, time(0));
	rfc1123_time(mdate, dir->mtime);

	fprint(fd, "HTTP/1.1 200 OK\n");
	fprint(fd, "Date: %s\n", date);
	fprint(fd, "Server: bijou/1.0 (Plan 9)\n");
	fprint(fd, "Last-Modified: %s\n", mdate);
	fprint(fd, "Content-Length: %.0f\n", (double)dir->length);
	//fprint(fd, "Content-Type: \n");

	fprint(fd, "\r\n");

	/* if the method was HEAD, return after printing the document header */

	if ( strcmp(req.method, "HEAD") == 0 ) {
		return 0;
	}

	/* read in the file and spit data back to the client */

	while((w = read(sfd, fbuf, sizeof(fbuf))) > 0) {
		write(fd, fbuf, w);
	}

	close(sfd);

	return 0;
}
