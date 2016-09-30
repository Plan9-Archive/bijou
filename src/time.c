/* bijou HTTP server for plan 9 by sean caron (scaron@umich.edu) */

#include "main.h"

/* generate a timestamp for log file entries */

void timestamp(char *ptr) {

	Tm *ti;

	ti = localtime(time(0));

	sprint(ptr, "%02d/%02d/%04d %02d:%02d:%02d", (ti->mon)+1, ti->mday, (ti->year)+1900,
		ti->hour, ti->min, ti->sec);

	return;
}

/* generate a timestamp suitable for a directory listing */

void file_time(char *ptr, ulong dtime) {

	Tm *ti;

	char month[4];

	ti = gmtime(dtime);

	switch (ti->mon) {

		case 0:
			strcpy(month, "Jan");
			break;

		case 1:
			strcpy(month, "Feb");
			break;

		case 2:
			strcpy(month, "Mar");
			break;

		case 3:
			strcpy(month, "Apr");
			break;

		case 4:
			strcpy(month, "May");
			break;

		case 5:
			strcpy(month, "Jun");
			break;

		case 6:
			strcpy(month, "Jul");
			break;

		case 7:
			strcpy(month, "Aug");
			break;

		case 8:
			strcpy(month, "Sep");
			break;

		case 9:
			strcpy(month, "Oct");
			break;

		case 10:
			strcpy(month, "Nov");
			break;

		case 11:
			strcpy(month, "Dec");
			break;
	}

	sprint(ptr, "%02d-%s-%04d %02d:%02d", ti->mday, month, (ti->year)+1900, ti->hour, ti->min);

	return;
}

/* generate a RFC1123 compliant timestamp for the Date header field */

void rfc1123_time(char *ptr, ulong tm) {

	Tm *ti;

	char day[4];
	char month[4];

	ti = gmtime(tm);

	switch (ti->wday) {

		case 0:
			strcpy(day, "Sun");
			break;

		case 1:
			strcpy(day, "Mon");
			break;

		case 2:
			strcpy(day, "Tue");
			break;

		case 3:
			strcpy(day, "Wed");
			break;

		case 4:
			strcpy(day, "Thu");
			break;

		case 5:
			strcpy(day, "Fri");
			break;

		case 6:
			strcpy(day, "Sat");
			break;
	}
	
	switch (ti->mon) {

		case 0:
			strcpy(month, "Jan");
			break;

		case 1:
			strcpy(month, "Feb");
			break;

		case 2:
			strcpy(month, "Mar");
			break;

		case 3:
			strcpy(month, "Apr");
			break;

		case 4:
			strcpy(month, "May");
			break;

		case 5:
			strcpy(month, "Jun");
			break;

		case 6:
			strcpy(month, "Jul");
			break;

		case 7:
			strcpy(month, "Aug");
			break;

		case 8:
			strcpy(month, "Sep");
			break;

		case 9:
			strcpy(month, "Oct");
			break;

		case 10:
			strcpy(month, "Nov");
			break;

		case 11:
			strcpy(month, "Dec");
			break;
	}

	sprint(ptr, "%s, %02d %s %04d %02d:%02d:%02d GMT", day, ti->mday, month, (ti->year)+1900, 
		ti->hour, ti->min, ti->sec);

	return;
}
