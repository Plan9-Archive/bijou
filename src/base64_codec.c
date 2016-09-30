/* bijou HTTP server for plan 9 by sean caron (scaron@umich.edu) */

#include "main.h"
#include "base64.h"

int base64_encode(char *ip, char **op) {

	unsigned long r = 0;

	int pi = 0;
	int po = 0;

	*op = malloc( 5*sizeof(char) );

	if ( *op == 0 ) {
		exits("MALLOC");
	}

	for ( pi = 0; pi < strlen(ip)+1; pi = pi + 3) {

		if ( ip[pi] == '\0') {

			*(*op + (4*po)) = '\0';
			break;
		}

		else if ( ip[pi+1] == '\0' ) {

			// two characters output, == pad

			r = (ip[pi] << 24);

			*(*op + (4*po)) = base64_lut[r >> 26];
			*(*op + ((4*po)+1)) = base64_lut[(r << 6) >> 26];
			*(*op + ((4*po)+2)) = '=';
			*(*op + ((4*po)+3)) = '=';

			*(*op + ((4*po)+4)) = '\0';

			break;
		}

		else if ( ip[pi+2] == '\0' ) {

			// three characters output, = pad

			r = (ip[pi] << 24) | (ip[pi+1] << 16);

			*(*op + (4*po)) = base64_lut[r >> 26];
			*(*op + ((4*po)+1)) = base64_lut[(r << 6) >> 26];
			*(*op + ((4*po)+2)) = base64_lut[(r << 12) >> 26];
			*(*op + ((4*po)+3)) = '=';

			*(*op + ((4*po)+4)) = '\0';

			break;
		}

		else {

			// four characters output, no pad

			r = (ip[pi] << 24) | (ip[pi+1] << 16) | (ip[pi+2] << 8);

			*(*op + (4*po)) = base64_lut[r >> 26];
			*(*op + ((4*po)+1)) = base64_lut[(r << 6) >> 26];
			*(*op + ((4*po)+2)) = base64_lut[(r << 12) >> 26];
			*(*op + ((4*po)+3)) = base64_lut[(r << 18) >> 26];
		}

		po = po + 1;

		*op = realloc(*op, sizeof(char)*((4*po)+5) );

		if ( *op == 0 ) {
			exits("REALLOC");
		}

	}
	
	return strlen(*op);
}

int base64_decode(char *ip, char **op) {

	unsigned long r = 0;

	unsigned char t[4] = {0, 0, 0, 0};

	int pi = 0;
	int po = 0;

	/* return with -1 to signal malformed input */

	if ( (strlen(ip) % 4) != 0 ) {
		return -1;
	}

	*op = malloc( 4*sizeof(char) );

	if ( *op == 0 ) {
		exits("MALLOC");
	}

	for ( pi = 0; pi < strlen(ip); pi = pi + 4) {

		t[0] = base64_ilut[ip[pi]] << 2;
		t[1] = base64_ilut[ip[pi+1]] << 2;
		t[2] = base64_ilut[ip[pi+2]] << 2;
		t[3] = base64_ilut[ip[pi+3]] << 2;

		r = t[0] << 24;
		r = r & 0xFC000000;

		r = r | (t[1] << 18);
		r = r & 0xFFF00000;

		r = r | (t[2] << 12);
		r = r & 0xFFFFC000;

		r = r | (t[3] << 6);
		r = r & 0xFFFFFF00;

		/* the characters are packed in the three high order bytes of long, r. the lower 8 bits of r are zero */

		*(*op + (3*po)) = (char)(r>>24);
		*(*op + ((3*po)+1)) = (char)((r<<8)>>24);
		*(*op + ((3*po)+2)) = (char)((r<<16)>>24);

		*(*op + ((3*po)+3)) = '\0';

		po = po + 1;

		*op = realloc(*op, sizeof(char)*((4*po)+5) );

		if ( *op == 0 ) {
			exits("REALLOC");
		}

	}

	return strlen(*op);
}
