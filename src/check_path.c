/* bijou HTTP server for plan 9 by sean caron (scaron@umich.edu) */

#include "main.h"

// if this returns 0 then the path tries to go below the root

int check_directory_path(char *path) {

	int i, d;

	d = 0;	// path depth counter

	i = 1;		// position-within-path-string counter

	while (1) {

		/* hit end of string; stop */

		if ( i >= strlen(path) ) {
			break;
		}

		// doing this lookbehind should catch the case of multiple concurrent slashes in the path

		if ( (path[i] == '/') && (path[i-1] != '/' ) ) {
			d = d + 1;
			i = i + 1;
		}

		/* handle .. in middle of string */

		else if ( (path[i] == '.') && (path[i-1] == '/') && (path[i+1] == '.') && (path[i+2] == '/') ) {
			i = i + 3;
			d = d - 1;

		}

		/* handle special case .. at end of string */

		else if ( (path[i] == '.') && (path[i-1] == '/') && (path[i+1] == '.') && ( (i+2) == strlen(path) ) ) {
			i = i + 3;
			d = d - 1;

		}

		/* regular character */

		else {
			i = i + 1;
		}

		/* fallen below the root; stop */

		if ( d < 0 ) {
			break;
		}

	}

	return d;
}
