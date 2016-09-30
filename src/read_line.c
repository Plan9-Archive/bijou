/* bijou HTTP server for plan 9 by sean caron (scaron@umich.edu) */

#include "main.h"

/* read in a line from the specified file descriptor to the specified buffer */

int read_line(int infd, char **ptr) {

	int len = 1;
	int n;

	*ptr = malloc(sizeof(char));

	while (1) {

		n = read(infd, &(*(*ptr+len-1)), 1);

		*ptr = realloc(*ptr, (len+1)*sizeof(char));

		if ( *ptr == 0 ) {
			exits("REALLOC");
		}

		if ( *(*ptr+len-1) == 10 ) {
			*(*ptr+len) = '\0';
			break;
		}

		/* disconnected */

		if ( n == 0 ) {
			exits("DISCONNECTED");
		}

		len = len + 1;

	}

	return len;
}
