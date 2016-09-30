/* bijou HTTP server for plan 9 by sean caron (scaron@umich.edu) */

#include "main.h"

int main(int argc, char **argv) {

	int dfd, acfd, lcfd, rlgfd, elgfd, tfd;
	char adir[40], ldir[40], *line, anc_buf[40];
	int r, rr, i, fi;

	Dir *dir;

	struct configuration cfg;
	struct request req;
	struct htpasswd htpass;

	char tstamp[32];

	char *path, *rpath, *temp;

	initialize_request_struct(&req);

	if ( argc != 3 ) {
		printf("usage: %s -f configfile\n", argv[0] );
		exits(0);
	}

	if ( strcmp(argv[1], "-f") == 0 ) {
		
		r = read_config_file(argv[2], &cfg);

		/* error code 1 is basically, failed to open file */
		/* error code 2 is parse error or undefined key in config file */
		/* error code 3 is duplicated configuration parameter */
		/* error code 4 is missing required configuration parameter */

		if ( r != 0 ) {
			printf("configuration file error %d\n", r);
			exits(0);
		}

	}

	else {
		printf("usage: %s -f configfile\n", argv[0] );
		exits(0);
	}

	/* remove trailing slash from various paths if it was included by the user */

	if ( cfg.rootdir[strlen(cfg.rootdir)-1] == '/' ) {
		cfg.rootdir[strlen(cfg.rootdir)-1] = '\0';
	}

	if ( cfg.userdir[strlen(cfg.userdir)-1] == '/' ) {
		cfg.userdir[strlen(cfg.userdir)-1] = '\0';
	}

	if ( cfg.reqlog[strlen(cfg.reqlog)-1] == '/' ) {
		cfg.reqlog[strlen(cfg.reqlog)-1] = '\0';
	}

	if ( cfg.errlog[strlen(cfg.errlog)-1] == '/' ) {
		cfg.errlog[strlen(cfg.errlog)-1] = '\0';
	}

	if ( cfg.icondir[strlen(cfg.icondir)-1] == '/' ) {
		cfg.icondir[strlen(cfg.icondir)-1] = '\0';
	}

	/* if logfiles exist append to them, otherwise create them */

	/* request log */

	if ( (rlgfd = open(cfg.reqlog, OWRITE)) < 0) {	

		if ( (rlgfd = create(cfg.reqlog, OWRITE, 0777)) < 0) {
		
			printf("failed to create request log\n");

			free_config_vars(&cfg);

			exits("CREATE");
		}
	}

	/* error log */

	if ( (elgfd = open(cfg.errlog, OWRITE)) < 0) {	

		if ( (elgfd = create(cfg.errlog, OWRITE, 0777)) < 0) {
		
			printf("failed to create erorr log\n");

			free_config_vars(&cfg);

			exits("CREATE");
		}
	}

	/* if the logfile already exists go to the end of the file, otherwise this has no effect */

	seek(rlgfd, 0, 2);
	seek(elgfd, 0, 2);

	timestamp(tstamp);
	fprint(rlgfd, "%s starting bijou HTTP server 1.0 on port %s\n", tstamp, cfg.port);
	fprint(elgfd, "%s starting bijou HTTP server 1.0 on port %s\n", tstamp, cfg.port);

	/* listen on the specified port for http clients */

	sprintf( anc_buf, "tcp!*!%s", cfg.port );

	acfd = announce(anc_buf, adir);
	
	if ( acfd < 0 ) {

		free_config_vars(&cfg);

		timestamp(tstamp);
		fprint(elgfd, "%s err: announce() failed\n", tstamp);
		close(rlgfd);
		close(elgfd);

		exits("ANNOUNCE");
	}

	while(1) {

		/* listen for a call */

		lcfd = listen(adir, ldir);
		
		if ( lcfd < 0 ) {

			free_config_vars(&cfg);

			timestamp(tstamp);
			fprint(elgfd, "%s err: listen() failed\n", tstamp);
			close(elgfd);
			close(rlgfd);

			exits("LISTEN");
		}

		/* fork a process to handle the request */

		switch( fork() ) {

			/* fork error */

			case -1:
				close(lcfd);

				break;
				
			/* child */

			case 0:

				/* accept the call and open the data file */

				dfd = accept(lcfd, ldir);

				if ( dfd < 0 ) {

					close(dfd);
					close(lcfd);

					timestamp(tstamp);
					fprint(elgfd, "%s err: accept() failed\n", tstamp);
					close(elgfd);
					close(rlgfd);

					exits("ACCEPT");
				}

				while (1) {

					/* read a line from the network */

					rr = read_line(dfd, &line);

					/* if we just get a newline, the request stream has ended */

					if ( rr <= 2 ) {
						break;
					}

					/* parse the line into the request structure */

					r = parse_request_line(line, &req);

					if ( r > 0 ) {
						break;
					}

				}

				/* check for null request */

				if ( (strcmp(req.method, "") == 0) && (strcmp(req.uri, "") == 0) && (strcmp(req.version, "") == 0) ) {
						close(rlgfd);
						close(elgfd);

						close(dfd);
						close(lcfd);

						free_request_vars(&req);

						exits(0);
				}

				/* decode URI encoding */

				uri_decode(req.uri, &temp);

				req.uri = realloc(req.uri, sizeof(char)*(strlen(temp)+1) );

				if ( req.uri == 0 ) {
					exits("REALLOC");
				}

				strcpy(req.uri, temp);

				free(temp);

				/* deal with various parsing errors */

				switch (r) {

					/* malformed request */

					case 1:
					
						timestamp(tstamp);
						fprint(elgfd, "%s err: malformed request\n", tstamp);

						close(rlgfd);
						close(elgfd);

						close(dfd);
						close(lcfd);

						free_request_vars(&req);

						exits(0);
						break;

					/* HTTP 400 trailing characters in request */

					case 2:

						http_400_following(dfd, cfg.hostname, cfg.port, req);

						timestamp(tstamp);
						fprint(elgfd, "%s err: http 400 trailing characters in request\n", tstamp);

						close(rlgfd);
						close(elgfd);

						close(dfd);
						close(lcfd);

						free(line);

						free_request_vars(&req);

						exits(0);
						break;

					/* HTTP 400 invalid URI in request */

					case 3:

						http_400_invalid(dfd, cfg.hostname, cfg.port, req);

						timestamp(tstamp);
						fprint(elgfd, "%s err: http 400 invalid uri in request\n", tstamp);

						close(elgfd);
						close(rlgfd);
					
						close(dfd);
						close(lcfd);

						free(line);

						free_request_vars(&req);

						exits(0);
						break;

					/* HTTP 501 unsupported method */

					case 4:

						http_501(dfd, cfg.hostname, cfg.port, req);

						timestamp(tstamp);
						fprint(elgfd, "%s err: http 501 unsupported method in request\n", tstamp);

						close(elgfd);
						close(rlgfd);

						close(dfd);
						close(lcfd);

						free(line);

						free_request_vars(&req);

						exits(0);
						break;
				}

				/* PATH GENERATION */

				/* basically, path is the full filesystem path to the requested file or directory */
				/* rpath is the root portion of the path, after translation */

				generate_path(&path, &rpath, &req, &cfg);

				/* AUTH CHECKING */

				if ( strcmp(cfg.htpassfile, "") != 0 ) {

					r = check_authorization(path, rpath, cfg, req, &htpass);

					switch (r) {

					case 1:

						timestamp(tstamp);
						fprint(elgfd, "%s err: failed to read or parse htpasswd file accessing %s\n", tstamp, req.uri);

						http_500(dfd, cfg.hostname, cfg.port, req);

						close(elgfd);
						close(rlgfd);

						close(dfd);
						close(lcfd);

						free(line);
						free(path);

						free(rpath);

						free_request_vars(&req);
						free_htpasswd_vars(&htpass);

						exits(0);
						break;

					case 2:

						timestamp(tstamp);
						fprint(elgfd, "%s err: authorization failed accessing %s\n", tstamp, req.uri);

						http_401(dfd, cfg.hostname, cfg.port, htpass.realm, req);

						close(elgfd);
						close(rlgfd);

						close(dfd);
						close(lcfd);

						free(line);
						free(path);

						free(rpath);

						free_request_vars(&req);
						free_htpasswd_vars(&htpass);

						exits(0);
						break;
					}

					free_htpasswd_vars(&htpass);
				}

				timestamp(tstamp);
				fprint(rlgfd, "%s req: %s\n", tstamp, req.uri);

				/* figure out if this is a file or a directory */

				dir = dirstat(path);

				/* try to exit gracefully if the file or directory does not exist */

				if ( dir == nil ) {

					timestamp(tstamp);
					fprint(elgfd, "%s err: no such file or directory %s\n", tstamp, req.uri);

					http_404(dfd, req.uri, cfg.hostname, cfg.port, req);

					close(elgfd);
					close(rlgfd);

					close(dfd);
					close(lcfd);

					free(line);
					free(path);

					free(rpath);

					free_request_vars(&req);

					exits(0);
				}

				/* directory */

				if ( (dir->mode & 0x80000000) == 0x80000000 ) {

					/* if no trailing slash specified on a directory, rewrite it with such and redirect the client */

					if ( path[strlen(path)-1] != '/' ) {

						req.uri = realloc(req.uri, sizeof(char)*(strlen(req.uri)+2));

						if ( req.uri == 0 ) {
							exits("REALLOC");
						}

						strcat(req.uri, "/");

						http_301(dfd, cfg.hostname, cfg.port, req);

						close(dfd);
						close(lcfd);

						close(elgfd);
						close(rlgfd);

						free(line);
						free(path);

						free(rpath);

						free_request_vars(&req);

						exits(0);
					}

					/* look for an index file. if we find it, note the offset in the indices array */

					fi = -1;

					for ( i = 0; i < cfg.n_indices; i++ ) {

						temp = malloc( (strlen(path)+strlen(*(cfg.indices+i))+4)*sizeof(char) );

						if ( temp == 0 ) {
							exits("MALLOC");
						}

						strcpy(temp, path);
						strcat(temp, *(cfg.indices+i));

						tfd = open(temp, OREAD);

						if ( tfd > 0 ) {
							close(tfd);

							fi = i;

							free(temp);

							break;
						}

						free(temp);
					}

					/* found an index, so use that */

					if ( fi != -1 ) {

						temp = malloc((strlen(path)+strlen(*(cfg.indices+fi))+4)*sizeof(char));

						if ( temp == 0 ) {
							exits("MALLOC");
						}

						strcpy(temp, path);

						strcat(temp, *(cfg.indices+fi) );

						path = realloc(path, (strlen(temp)+1)*sizeof(char));

						if ( path == 0 ) {
							exits("REALLOC");
						}

						strcpy(path, temp);

						free(temp);

						dir = dirstat(path);

						r = serve_file(path, dir, dfd, req);

						if ( r != 0 ) {
							timestamp(tstamp);
							fprint(elgfd, "%s err: failed to open %s\n", tstamp, req.uri);

							http_404(dfd, req.uri, cfg.hostname, cfg.port, req);

							close(elgfd);
							close(rlgfd);

							close(dfd);
							close(lcfd);

							free(line);
							free(path);

							free(rpath);

							free_request_vars(&req);

							exits(0);
						}
					}

					/* no index, keep the path as just the directory so we can list it */

					else {

						r = serve_dir(path, req.uri, cfg.hostname, cfg.port, cfg.icondir, dir, dfd, req, cfg);

						if ( r != 0 ) {

							timestamp(tstamp);
							fprint(elgfd, "%s err: failed to open %s\n", tstamp, req.uri );

							http_404(dfd, req.uri, cfg.hostname, cfg.port, req);

							close(elgfd);
							close(rlgfd);

							close(dfd);
							close(lcfd);

							free(line);
							free(path);

							free(rpath);

							free_request_vars(&req);

							exits(0);
						}
					}
				}			

				/* file */

				else {

					// if the client requests a htpasswd file, just give them a 403 forbidden error

					if ( strstr(path, cfg.htpassfile) != 0 ) {
						timestamp(tstamp);
						fprint(elgfd, "%s err: forbidden to access %s\n", tstamp, req.uri);

						http_403(dfd, cfg.hostname, cfg.port, req);

						close(elgfd);
						close(rlgfd);

						close(dfd);
						close(lcfd);

						free(line);
						free(path);

						free(rpath);

						free_request_vars(&req);

						exits(0);
					}

					// otherwise try to serve the file

					r = serve_file(path, dir, dfd, req);

					if ( r != 0 ) {
						timestamp(tstamp);
						fprint(elgfd, "%s err: failed to open %s\n", tstamp, req.uri);

						http_404(dfd, req.uri, cfg.hostname, cfg.port, req);

						close(elgfd);
						close(rlgfd);

						close(dfd);
						close(lcfd);

						free(line);
						free(path);

						free(rpath);

						free_request_vars(&req);

						exits(0);
					}

				}

				close(dfd);
				close(lcfd);

				close(elgfd);
				close(rlgfd);

				free(line);
				free(path);

				free(rpath);

				free_request_vars(&req);

				exits(0);
				break;

			/* parent */

			default:
				close(lcfd);

				break;

		}

	}
}
