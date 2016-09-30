/* bijou HTTP server for plan 9 by sean caron (scaron@umich.edu) */

#include "main.h"

/* read in configuration file, use the data to initialize server configuration structure */

/* error code 0 is return with no errors, input OK */
/* error code 1 is basically, failed to open file */
/* error code 2 is parse error or undefined key in config file */
/* error code 3 is duplicated configuration parameter */
/* error code 4 is missing required configuration parameter */

int read_config_file(char *path, struct configuration *cfg) {

	int n, fd;
	char *line, *key, *value, *temp;

	int defined[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

	int len = 1;

	/* initialize some configuration variables */

	cfg->n_translate = 0;
	cfg->n_indices = 0;

	cfg->htpassfile = malloc(sizeof(char));

	if ( cfg->htpassfile == 0 ) {
		exits("MALLOC");
	}

	strcpy( cfg->htpassfile, "" );

	/* attempt to open the configuration file */
	
	fd = open(path, OREAD);

	/* if the file does not exist, return to the caller with an error code */

	if ( fd < 0 ) {
		return 1;
	}

	/* if it exists, read it in */

	line = malloc(sizeof(char));

	while (1) {

		n = read(fd, &(*(line+len-1)), 1);

		line = realloc(line, (len+1)*sizeof(char));

		if ( line == 0 ) {
			exits("REALLOC");
		}

		/* finished reading a line */

		if ( *(line+len-1) == '\n' ) {

			/* terminate it */

			*(line+len-1) = '\0';

			/* exclude comments, blank lines, and lines starting with invalid characters */

			if ( ( line[0] != '#' ) && (strlen(line)>0) && isalpha(line[0]) ) {

				/* allocate some memory for the key and value */

				key = malloc( sizeof(char)*(strlen(line)+1) );

				if ( key == 0 ) {
					exits("MALLOC");
				}

				value = malloc( sizeof(char)*(strlen(line)+1));

				if ( value == 0 ) {
					exits("MALLOC");
				}

				/* parse each line into key=value pairs */

				temp = strtok(line, "=");

				/* parse error */

				if ( temp == 0 ) {
					free(key);
					free(value);
					free(line);

					return 2;
				}

				else {
					strcpy(key, temp);
				}

				temp = strtok(0, "=");

				/* parse error */

				if ( temp == 0 ) {
					free(key);
					free(value);
					free(line);

					return 2;
				}

				else {
					strcpy(value, temp);
				}

				/* port */

				if ( strcmp(key, "port") == 0 ) {

					/* check for already defined */

					if ( defined[0] == 1 ) {
						free(key);
						free(value);
						free(line);

						return 3;
					}

					defined[0] = 1;

					cfg->port = malloc( sizeof(char)*(strlen(value)+1));

					if ( cfg->port == 0 ) {
						exits("MALLOC");
					}

					strcpy(cfg->port, value);
				}

				/* host name */

				else if ( strcmp(key, "hostname") == 0 ) {

					/* check for already defined */

					if ( defined[1] == 1 ) {
						free(key);
						free(value);
						free(line);

						return 3;
					}

					defined[1] = 1;

					cfg->hostname = malloc( sizeof(char)*(strlen(value)+1));

					if ( cfg->hostname == 0 ) {
						exits("MALLOC");
					}

					strcpy(cfg->hostname, value);
				}

				/* icon directory */

				else if ( strcmp(key, "icondir") == 0 ) {

					/* check for already defined */

					if ( defined[2] == 1 ) {
						free(key);
						free(value);
						free(line);

						return 3;
					}

					defined[2] = 1;

					cfg->icondir = malloc( sizeof(char)*(strlen(value)+1));

					if ( cfg->icondir == 0 ) {
						exits("MALLOC");
					}

					strcpy(cfg->icondir, value);
				}

				/* root directory */

				else if ( strcmp(key, "rootdir") == 0 ) {

					/* check for already defined */

					if ( defined[3] == 1 ) {
						free(key);
						free(value);
						free(line);

						return 3;
					}

					defined[3] = 1;

					cfg->rootdir = malloc( sizeof(char)*(strlen(value)+1));

					if ( cfg->rootdir == 0 ) {
						exits("MALLOC");
					}

					strcpy(cfg->rootdir, value);
				}

				/* index file */

				else if ( strcmp(key, "indices") == 0 ) {

					/* check for already defined */

					if ( defined[4] == 1 ) {
						free(key);
						free(value);
						free(line);

						return 3;
					}

					defined[4] = 1;

					/* check for trailing comma, we want to catch this because it would confuse strtok */

					if ( temp[strlen(value)-1] == ',' ) {

						free(key);
						free(value);
						free(line);

						return 2;
					}

					cfg->indices = malloc( sizeof(char *) );

					if ( cfg->indices == 0 ) {
						exits("MALLOC");
					}

					*(cfg->indices) = malloc( sizeof(char)*(strlen(value)+1) );

					if ( *(cfg->indices) == 0 ) {
						exits("MALLOC");
					}

					cfg->n_indices = 1;

					temp = malloc( sizeof(char)*(strlen(value)+1) );

					if ( temp == 0 ) {
						exits("MALLOC");
					}

					strcpy(temp, value);

					/* single index file */

					if ( strtok(temp, ",") == 0 ) {

						free(temp);

						strcpy( *(cfg->indices), value );

					}

					/* multiple index files */

					else {

						free(temp);

						temp = strtok(value, ",");

						strcpy( *(cfg->indices), temp );

						while (1) {

							temp = strtok(0, ",");

							if ( temp != 0 ) {

								cfg->indices = realloc(cfg->indices, (sizeof(char *)*(cfg->n_indices+2)));

								if ( cfg->indices == 0 ) {
									exits("REALLOC");
								}

								*(cfg->indices+cfg->n_indices) = malloc(sizeof(char)*(strlen(value)+2));

								if ( *(cfg->indices+cfg->n_indices) == 0 ) {
									exits("MALLOC");
								}

								strcpy( *(cfg->indices+cfg->n_indices), temp );

								cfg->n_indices++;
							}

							else {

								break;
							}
						}


					}
				}

				/* directory translation */

				else if ( strcmp(key, "translate") == 0 ) {

					/* check for trailing comma, we want to catch this because it would confuse strtok */

					if ( temp[strlen(value)-1] == ',' ) {

						free(key);
						free(value);
						free(line);

						return 2;
					}

					temp = malloc( sizeof(char)*(strlen(value)+1) );

					if ( temp == 0 ) {
						exits("MALLOC");
					}

					strcpy(temp, value);

					/* if strtok() on comma fails then the line is malformed */

					if ( strtok(temp, ",") == 0 ) {

						free(key);
						free(value);
						free(line);

						free(temp);

						return 2;

					}

					free(temp);

					if ( cfg->n_translate == 0 ) {
						cfg->translate = malloc( 2*sizeof(char *) );
					}

					else {
						cfg->translate = realloc(cfg->translate, (sizeof(char *)*((cfg->n_translate*2)+2)) );
					}
						

					if ( cfg->translate == 0 ) {
						exits("MALLOC");
					}

					/* allocate memory for the translation pair */

					*(cfg->translate+(2*cfg->n_translate)) = malloc( sizeof(char)*(strlen(value)+1) );

					if ( *(cfg->translate+(2*cfg->n_translate)) == 0 ) {
						exits("MALLOC");
					}

					*(cfg->translate+((2*cfg->n_translate)+1)) = malloc( sizeof(char)*(strlen(value)+1) );

					if ( *(cfg->translate+((2*cfg->n_translate)+1)) == 0 ) {
						exits("MALLOC");
					}

					temp = strtok(value, ",");

					strcpy( *(cfg->translate+(2*cfg->n_translate)), temp);

					temp = strtok(0, ",");

					strcpy( *(cfg->translate+((2*cfg->n_translate)+1)), temp);

					cfg->n_translate++;
				}

				/* user directory character */

				else if ( strcmp(key, "userchar") == 0 ) {

					/* check for already defined */

					if ( defined[5] == 1 ) {
						free(key);
						free(value);
						free(line);

						return 3;
					}

					defined[5] = 1;

					cfg->userchar = malloc( sizeof(char)*(strlen(value)+1));

					if ( cfg->userchar == 0 ) {
						exits("MALLOC");
					}

					strcpy(cfg->userchar, value);
				}

				/* user directory prefix */

				else if ( strcmp(key, "userdir") == 0 ) {

					/* check for already defined */

					if ( defined[6] == 1 ) {
						free(key);
						free(value);
						free(line);

						return 3;
					}

					defined[6] = 1;

					cfg->userdir = malloc( sizeof(char)*(strlen(value)+1));

					if ( cfg->userdir == 0 ) {
						exits("MALLOC");
					}

					strcpy(cfg->userdir, value);
				}

				/* user html directory name */

				else if ( strcmp(key, "userdocs") == 0 ) {

					/* check for already defined */

					if ( defined[7] == 1 ) {
						free(key);
						free(value);
						free(line);

						return 3;
					}

					defined[7] = 1;

					cfg->userdocs = malloc( sizeof(char)*(strlen(value)+1));

					if ( cfg->userdocs == 0 ) {
						exits("MALLOC");
					}

					strcpy(cfg->userdocs, value);
				}

				/* request log file */

				else if ( strcmp(key, "reqlog") == 0 ) {

					/* check for already defined */

					if ( defined[8] == 1 ) {
						free(key);
						free(value);
						free(line);

						return 3;
					}

					defined[8] = 1;

					cfg->reqlog = malloc( sizeof(char)*(strlen(value)+1));

					if ( cfg->reqlog == 0 ) {
						exits("MALLOC");
					}

					strcpy(cfg->reqlog, value);
				}

				/* error log file */

				else if ( strcmp(key, "errlog") == 0 ) {

					/* check for already defined */

					if ( defined[9] == 1 ) {
						free(key);
						free(value);
						free(line);

						return 3;
					}

					defined[9] = 1;

					cfg->errlog = malloc( sizeof(char)*(strlen(value)+1));

					if ( cfg->errlog == 0 ) {
						exits("MALLOC");
					}

					strcpy(cfg->errlog, value);
				}

				/* htpasswd file name */

				else if ( strcmp(key, "htpassfile") == 0 ) {

					cfg->htpassfile = realloc( cfg->htpassfile, sizeof(char)*(strlen(value)+1));

					if ( cfg->htpassfile == 0 ) {
						exits("REALLOC");
					}

					strcpy(cfg->htpassfile, value);
				}

				/* nominally valid line format but bad key */

				else {
					free(key);
					free(value);
					free(line);

					return 2;
				}

				free(key);
				free(value);

			}

			/* reset the buffer for the next input line */

			free(line);
			line = malloc(sizeof(char));

			if ( line == 0 ) {
				exits("MALLOC");
			}

			len = 1;
		}

		else {

			len = len + 1;
		}

		/* eof */

		if ( n == 0 ) {
			break;
		}
	}

	close(fd);

	free(line);

	/* check for missing required parameters */

	for ( n = 0; n < 10; n++ ) {
		if ( defined[n] != 1 ) {
			return 4;
		}
	}

	return 0;
}
