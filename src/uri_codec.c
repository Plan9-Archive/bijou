/* bijou HTTP server for plan 9 by sean caron (scaron@umich.edu) */

#include "main.h"

int uri_encode(char *ip, char **op) {

	int i, j, t;
	char temp[4];
	int ol;

	ol = strlen(ip);

	*op = malloc ( sizeof(char) * (ol+1) );

	if ( *op == 0 ) {
		exits("MALLOC");
	}

	i = 0;
	j = 0;

	/* catch null input */

	if ( strlen(ip) == 0 ) {
		strcpy(*op, "");
		return 0;
	}

	while (1) {

		/* just pass unreserved characters through with no encoding */

		if ( ((ip[i] >= 43) && (ip[i] <= 47)) || (ip[i] == 95) || (ip[i] == 126) || ( (ip[i] >= 48) && (ip[i] <= 57) ) || ( (ip[i] >= 65) && (ip[i] <= 90) ) || ( (ip[i] >= 97) && (ip[i] <= 122) ) ) {

			*(*op+j) = ip[i];

			i = i + 1;
			j = j + 1;
		}

		/* otherwise express it in the uri encoded format */

		else {

			ol = ol + 3;

			*op = realloc(*op, sizeof(char)*(ol+1) );

			if ( *op == 0 ) {
				exits("REALLOC");
			}

			sprintf(temp, "%2.0x", ip[i] );

			*(*op+j) = '%';
			*(*op+j+1) = temp[0];
			*(*op+j+2) = temp[1];

			i = i + 1;
			j = j + 3;
		}

		if ( i == strlen(ip) ) {
			break;
		}

	}

	*(*op+j) = '\0';

	return 0;
	
}

int uri_decode(char *ip, char **op) {

	int i, j, t;
	char temp[4];

	*op = malloc ( sizeof(char) * (strlen(ip)+1) );

	if ( *op == 0 ) {
		exits("MALLOC");
	}

	i = 0;
	j = 0;

	/* catch null input */

	if ( strlen(ip) == 0 ) {
		strcpy(*op, "");
		return 0;
	}

	while (1) {

		if ( (ip[i] == '%') && isxdigit(ip[i+1]) && isxdigit(ip[i+2]) ) {

			// ascii value of the encoded char is in ip[i+1] and ip[i+2] in hex; ip[i+1] is MSB.

			sprintf(temp, "%c%c", ip[i+1], ip[i+2] );

			t = (int)strtol(temp, (char**)NULL, 16);

			*(*op+j) = (char)t;

			i = i + 3;
			j = j + 1;
		}

		else {

			*(*op+j) = ip[i];

			i = i + 1;
			j = j + 1;

		}

		if ( i == strlen(ip) ) {
			break;
		}

	}

	*(*op+j) = '\0';

	return 0;
	
}
