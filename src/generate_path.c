/* bijou HTTP server for plan 9 by sean caron (scaron@umich.edu) */

#include "main.h"

void generate_path(char **path, char **rpath, struct request *req, struct configuration *cfg) {

	char *temp, *head, *tail, *tu;

	int i, j, ft;

	/* check for user directory expansion */

	if ( req->uri[1] == *(cfg->userchar) ) {
			
		/* work up some temporaries for user directory expansion checks */

		head = malloc(sizeof(char)*(strlen(req->uri)+1));

		if ( head == 0 ) {
			exits("MALLOC");
		}

		tail = malloc(sizeof(char)*(strlen(req->uri)+1));

		if ( tail == 0 ) {
			exits("MALLOC");
		}

		tu = malloc(sizeof(char)*(strlen(req->uri)+1));

		if ( tu == 0 ) {
			exits("MALLOC");
		}

		strcpy(tu, req->uri);

		temp = strtok(tu, "/");

		if ( temp == 0 ) {
			strcpy(head, "");
		}

		else {
			strcpy(head, temp);
		}

		temp = strtok(0, "");

		if ( temp == 0 ) {

			/* 
			  * strtok obscures whether or not there was a trailing slash followed by nothing else after the
			  * first token is chopped in our temporary copy of the URI. we must go back to the original
			  * copy of the URI to check for trailing slash. if there was a trailing slash in the URI, reappend
			  * it so as to avoid erroneously triggering an HTTP 301 redirect later
			*/

			if ( req->uri[strlen(head)+1] == '/' ) {
				strcpy(tail, "/");
			}

			/* otherwise set tail to null string. the request will (correctly) trigger HTTP 301 later */

			else {
				strcpy(tail, "");
			}
		}

		else {
			sprintf(tail, "/%s", temp);
		}

		/* deal with attempts to get below the user html directory */

		if ( (strcmp(tail, "/..") == 0) || (strcmp(tail, "/../") == 0) ) {

			*path = malloc( sizeof(char)*(strlen(cfg->rootdir)+2) );

			if ( *path == 0 ) {
				exits("MALLOC");
			}

			sprintf(*path, "%s/", cfg->rootdir );

			req->uri = realloc(req->uri, 2*sizeof(char));

			if ( req->uri == 0 ) {
				exits("REALLOC");
			}

			strcpy(req->uri, "/");

			*rpath = malloc(sizeof(char)*(strlen(cfg->rootdir)+1));

			if ( *rpath == 0 ) {
				exits("MALLOC");
			}

			strcpy(*rpath, cfg->rootdir);
		}

		/* otherwise construct the translated path as usual */

		else {
			strcpy(head, strtok(head, cfg->userchar));

			*path = malloc( sizeof(char)*(strlen(cfg->userdir)+strlen(head)+strlen(cfg->userdocs)+strlen(tail)+4) );

			if ( *path == 0 ) {
				exits("MALLOC");
			}

			sprintf(*path, "%s/%s/%s%s", cfg->userdir, head, cfg->userdocs, tail);

			*rpath = malloc(sizeof(char)*(strlen(cfg->userdir)+strlen(head)+strlen(cfg->userdocs)+4) );

			if (*rpath == 0 ) {
				exits("MALLOC");
			}

			sprintf(*rpath, "%s/%s/%s", cfg->userdir, head, cfg->userdocs);
		}

		/* free translation path assembly temporaries */

		free(tu);
		free(head);
		free(tail);

	}

	/* path generation, no user directory expansion */

	else {

		ft = -1;

		/* check directory translations */

		for ( i = 0; i < cfg->n_translate; i++ ) {

			temp = malloc( sizeof(char)*(strlen(req->uri)+2) );

			if ( temp == 0 ) {
				exits("MALLOC");
			}

			/* if uri is at least as long as the translation */

			if ( strlen(req->uri) >= strlen(*(cfg->translate+(2*i))) ) {

				/* and the first [translation length] characters in the uri are followed immediately by a slash */
				/* or there is an exact match between the uri and the translation key in length */

				if ( (*(req->uri+strlen(*(cfg->translate+(2*i)))) == '/' ) || (strlen(req->uri) == strlen(*(cfg->translate+(2*i)))) ) {

					/* copy that substring to temp */

					strncpy(temp, req->uri, strlen( *(cfg->translate+(2*i))) );
					temp[strlen(*(cfg->translate+(2*i)))] = '\0';

					/* and if there is an exact match between temp and the translation */

					if ( strcmp(temp, *(cfg->translate+(2*i)) ) == 0 ) {

						/* then we found a match; flag it and break out of the loop */

						ft = i;
						free(temp);
						break;
					}
				}
			}

			free(temp);
		}

		/* translation, no user directory expansion */

		if ( ft != -1 ) {

			temp = malloc( sizeof(char)*(strlen(req->uri)+2) );

			if ( temp == 0 ) {
				exits("MALLOC");
			}

			j = 0;

			for ( i = strlen(*(cfg->translate+(2*ft))); i < strlen(req->uri); i++) {

				*(temp+j) = *(req->uri+i);
				j++;

			}

			*(temp+j) = '\0';

			/* deal with attempts to get below the translated root */

			if ( (strcmp(temp, "/..") == 0) || (strcmp(temp, "/../") == 0) ) {
				*path = malloc( sizeof(char)*(strlen(cfg->rootdir)+2) );

				if ( *path == 0 ) {
					exits("MALLOC");
				}

				sprintf(*path, "%s/", cfg->rootdir );

				req->uri = realloc(req->uri, 2*sizeof(char));

				if ( req->uri == 0 ) {
					exits("REALLOC");
				}

				strcpy(req->uri, "/");

				*rpath = malloc( sizeof(char)*(strlen(cfg->rootdir)+1) );

				if ( *rpath == 0 ) {
					exits("MALLOC");
				}

				strcpy(*rpath, cfg->rootdir);
			}

			/* otherwise apply translation as usual */

			else {

				*path = malloc( (strlen(*(cfg->translate+((2*ft)+1)))+strlen(temp)+2)*sizeof(char) );

				if ( *path == 0 ) {
					exits("MALLOC");
				}

				sprintf(*path, "%s%s", *(cfg->translate+((2*ft)+1)), temp );

				*rpath = malloc( sizeof(char)*(strlen(*(cfg->translate+((2*ft)+1)))+2) );

				if ( *rpath == 0 ) {
					exits("MALLOC");
				}

				strcpy(*rpath, *(cfg->translate+((2*ft)+1)) );
			}

			free(temp);

		}

		/* no user directory expansion, no translation */

		else {

			/* allocate some memory for the real file path */

			*path = malloc((strlen(cfg->rootdir)+strlen(req->uri)+1)*sizeof(char));

			if ( path == 0 ) {
				exits("MALLOC");
			}

			/* catenate the http server directory root with the uri to get a real file path */

			sprintf(*path, "%s%s", cfg->rootdir, req->uri );

			*rpath = malloc(sizeof(char)*(strlen(cfg->rootdir)+2) );

			if ( *rpath == 0 ) {
				exits("MALLOC");
			}

			strcpy(*rpath, cfg->rootdir);

		}
	}

	return;
}
