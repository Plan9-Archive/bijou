/* bijou HTTP server for plan 9 by sean caron (scaron@umich.edu) */

#include "main.h"

/* parse request line into a request structure with method, uri, and version
  *
  * return codes:
  *	0 : sucess
  *	1 : totally malformed method
  *	2 : http 400 trailing chars
  *	3 : http 400 invalid uri
  *	4 : http 501 invalid method
*/

int parse_request_line(char *line, struct request *req) {

	char *temp;
	char *de;

	int i, r;

	/* strip CR+LF from input */

	for (i = 0; i < strlen(line); i++) {
		if ( line[i] == '\r' ) {
			line[i] = '\0';
		}
	}

	/* strip tabs from input and replace them with spaces */

	for ( i = 0; i < strlen(line); i++) {
		if ( line[i] == '\t' ) {
			line[i] = ' ';
		}
	}

	/* need to parse out the method, uri, and version */

	temp = strtok(line, " ");

	/* if request is so malformed we can't even start to parse it, just exit gracefully */

	if ( temp == 0 ) {
		return 1;
	}

	// check to see if this is a parameter line
	// and that we have already had a proper request line previously

	if (  (temp[strlen(temp)-1] == ':') && (strlen(req->method) != 0) ) {

		if ( strcmp(temp, "Host:") == 0 ) {

			temp = strtok(0, "");

			if ( temp != 0 ) {

				req->host = realloc(req->host, sizeof(char)*(strlen(temp)+1) );

				if ( req->host == 0 ) {
					exits("REALLOC");
				}

				strcpy(req->host, temp);
			}

			// else just keep it null, as initialized
		}

		else if ( strcmp(temp, "User-Agent:") == 0 ) {

			temp = strtok(0, "");

			if ( temp != 0 ) {

				req->useragent = realloc(req->useragent, sizeof(char)*(strlen(temp)+1));

				if ( req->useragent == 0 ) {
					exits("REALLOC");
				}

				strcpy(req->useragent, temp);
			}

			// else just keep it null, as initialized

		}

		else if ( strcmp(temp, "Authorization:") == 0 ) {

			temp = strtok(0, " ");

			if ( temp != 0 ) {

				req->authtype = realloc(req->authtype, sizeof(char)*(strlen(temp)+1));

				if ( req->authtype == 0 ) {
					exits("REALLOC");
				}

				strcpy(req->authtype, temp);

				temp = strtok(0, "");

				if ( temp != 0 ) {

					req->authorization = realloc(req->authorization, sizeof(char)*(strlen(temp)+1));

					if ( req->authorization == 0 ) {
						exits("REALLOC");
					}

					base64_decode(temp, &de);

					strcpy(req->authorization, de);

					free(de);
				}

			}
		}

		// else we just ignore it
	}

	/* otherwise this is the request line */

	else {

		/* otherwise start the parsing process by taking the first token as the method */

		req->method = realloc(req->method, sizeof(char)*(strlen(temp)+1));

		if ( req->method == 0 ) {
			exits("REALLOC");
		}

		strcpy(req->method, temp);

		temp = strtok(0, " ");

		/* if the request was just GET with no URI then assume the URI to be the web root directory */

		if ( temp == 0 ) {

			req->uri = realloc(req->uri, 2*sizeof(char) );

			if ( req->uri == 0 ) {
				exits("REALLOC");
			}

			strcpy(req->uri, "/");
		}

		else {

			req->uri = realloc(req->uri, sizeof(char)*(strlen(temp)+1) );

			if ( req->uri == 0 ) {
				exits("REALLOC");
			}

			strcpy(req->uri, temp);
		}

		temp = strtok(0, " ");

		/* if version was not specified then just fill it in with a null string */

		if ( temp == 0 ) {

			req->version = realloc(req->version, sizeof(char) );

			if ( req-> version == 0 ) {
				exits("REALLOC");
			}

			strcpy(req->version, "");
		}

		else {

			req->version = realloc(req->version, sizeof(char)*(strlen(temp)+1) );

			if ( req->version == 0 ) {
				exits("MALLOC");
			}

			strcpy(req->version, temp);
		}

		/* check for out of spec trailing characters in request */

		if ( strtok(0, " ") != 0 ) {
			return 2;
		}

		/* check for invalid request version format */

		temp = malloc( sizeof(char)*(strlen(req->version)+1) );

		if ( temp == 0 ) {
			exits("MALLOC");
		}

		strcpy(temp, req->version);

		if ( (check_request_version(temp) != 0) && (strcmp(req->version, "") != 0) ) {
			return 2;
		}

		free(temp);

		/* check for invalid URI in request */

		if (req->uri[0] != '/' ) {

			return 3;
		}	

		/* check to see if the request tries to go below the web root */

		r = check_directory_path(req->uri);

		/* if so flag the request as an invalid URI */

		if ( r < 0 ) {
			return 3;
		}

		/* check for unsupported method in request. right now we only support GET and HEAD */

		if ( (strcmp(req->method, "GET") != 0) && (strcmp(req->method, "HEAD") != 0) ) {
			return 4;
		}
	}

	/* otherwise return with success */

	return 0;
}
