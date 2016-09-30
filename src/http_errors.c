/* bijou HTTP server for plan 9 by sean caron (scaron@umich.edu) */

#include "main.h"

/* HTTP 301 redirect */

void http_301(int fd, char *hostname, char *port, struct request req) {

	char date[32];
	char *tu;

	rfc1123_time(date, time(0));

	uri_encode(req.uri, &tu);

	fprint(fd, "HTTP/1.1 301 Moved Permanently\n");
	fprint(fd, "Date: %s\n", date );
	fprint(fd, "Server: bijou/1.0 (Plan 9)\n");
	fprint(fd, "Location: http://%s%s\n", hostname, tu);
	fprint(fd, "Content-Type: text/html\n");

	fprint(fd, "\r\n");

	/* if the method was HEAD, return after printing the document header */

	if ( strcmp(req.method, "HEAD") == 0 ) {
		return;
	}

	fprint(fd, "<HTML><HEAD>\n");
	fprint(fd, "<TITLE>301 Moved Permanently</TITLE>\n");
	fprint(fd, "</HEAD><BODY>\n");
	fprint(fd, "<H1>Moved Permanently</H1>\n");
	fprint(fd, "The document has moved <A HREF=\"http://%s%s\">here</A>.<P>\n", hostname, tu);
	fprint(fd, "<HR>\n");
	fprint(fd, "<ADDRESS>bijou/1.0 server at %s port %s</ADDRESS>\n", hostname, port);
	fprint(fd, "</BODY></HTML>\n");

	free(tu);

	return;
}

/* HTTP 400 bad request - characters following request */

void http_400_following(int fd, char *hostname, char *port, struct request req) {

	char date[32];

	rfc1123_time(date, time(0));

	fprint(fd, "HTTP/1.1 400 Bad Request\n");
	fprint(fd, "Date: %s\n", date);
	fprint(fd, "Server: bijou/1.0 (Plan 9)\n");
	fprint(fd, "Content-Type: text/html\n");

	fprint(fd, "\r\n");

	/* if the method was HEAD, return after printing the document header */

	if ( strcmp(req.method, "HEAD") == 0 ) {
		return;
	}

	fprint(fd, "<HTML><HEAD>\n");
	fprint(fd, "<TITLE>400 Bad Request</TITLE>\n");
	fprint(fd, "</HEAD><BODY>\n");
	fprint(fd, "<H1>Bad Request</H1>\n");
	fprint(fd, "Your browser sent a request that this server could not understand.<P>\n");
	fprint(fd, "The request line contained invalid characters following the protocol string.<P>\n");
	fprint(fd, "<P>\n");
	fprint(fd, "<HR>\n");
	fprint(fd, "<ADDRESS>bijou/1.0 server at %s port %s</ADDRESS>\n", hostname, port);
	fprint(fd, "</BODY></HTML>\n");

	return;
}

/* HTTP 400 bad request - invalid URI */

void http_400_invalid(int fd, char *hostname, char *port, struct request req) {

	char date[32];

	rfc1123_time(date, time(0));

	fprint(fd, "HTTP/1.1 400 Bad Request\n");
	fprint(fd, "Date: %s\n", date);
	fprint(fd, "Server: bijou/1.0 (Plan 9)\n");
	fprint(fd, "Content-Type: text/html\n");

	fprint(fd, "\r\n");

	/* if the method was HEAD, return after printing the document header */

	if ( strcmp(req.method, "HEAD") == 0 ) {
		return;
	}

	fprint(fd, "<HTML><HEAD>\n");
	fprint(fd, "<TITLE>400 Bad Request</TITLE>\n");
	fprint(fd, "</HEAD><BODY>\n");
	fprint(fd, "<H1>Bad Request</H1>\n");
	fprint(fd, "Your browser sent a request that this server could not understand.<P>\n");

	if ( strcmp(req.version,"") == 0 ) {
		fprint(fd, "Invalid URI in request %s %s<P>\n", req.method, req.uri);
	}

	else {
		fprint(fd, "Invalid URI in request %s %s %s<P>\n", req.method, req.uri, req.version);
	}

	fprint(fd, "<P>\n");
	fprint(fd, "<HR>\n");
	fprint(fd, "<ADDRESS>bijou/1.0 server at %s port %s</ADDRESS>\n", hostname, port);
	fprint(fd, "</BODY></HTML>\n");

	return;
}

/* HTTP 401 authorization required */

void http_401(int fd, char *hostname, char *port, char *realm, struct request req) {

	char date[32];

	rfc1123_time(date, time(0));

	fprint(fd, "HTTP/1.1 401 Authorization Required\n");
	fprint(fd, "Date: %s\n", date);
	fprint(fd, "Server: bijou/1.0 (Plan 9)\n");
	fprint(fd, "WWW-Authenticate: Basic realm=\"%s\"\n", realm);
	fprint(fd, "Content-Type: text/html\n");

	fprint(fd, "\r\n");

	/* if the method was HEAD, return after printing the document header */

	if ( strcmp(req.method, "HEAD") == 0 ) {
		return;
	}

	fprint(fd, "<HTML><HEAD>\n");
	fprint(fd, "<TITLE>401 Authorization Required</TITLE>\n");
	fprint(fd, "</HEAD><BODY>\n");
	fprint(fd, "<H1>Authorization Required</H1>\n");
	fprint(fd, "The server could not verify that you are authorized to access the document requested.\n");
	fprint(fd, "Either you supplied the wrong credentials, or your browser doesn't understand how to supply the credentials required.<P>\n");
	fprint(fd, "<HR>\n");
	fprint(fd, "<ADDRESS>bijou/1.0 server at %s port %s</ADDRESS>\n", hostname, port);
	fprint(fd, "</BODY></HTML>\n");

	return;
}

/* HTTP 403 forbidden */

void http_403(int fd, char *hostname, char *port, struct request req) {

	char date[32];

	rfc1123_time(date, time(0));

	fprint(fd, "HTTP/1.1 403 Forbidden\n");
	fprint(fd, "Date: %s\n", date);
	fprint(fd, "Server: bijou/1.0 (Plan 9)\n");
	fprint(fd, "Content-Type: text/html\n");

	fprint(fd, "\r\n");

	/* if the method was HEAD, return after printing the document header */

	if ( strcmp(req.method, "HEAD") == 0 ) {
		return;
	}

	fprint(fd, "<HTML><HEAD>\n");
	fprint(fd, "<TITLE>403 Forbidden</TITLE>\n");
	fprint(fd, "</HEAD><BODY>\n");
	fprint(fd, "<H1>Forbidden</H1>\n");
	fprint(fd, "You don't have permission to access %s on this server.<P>\n", req.uri);
	fprint(fd, "<HR>\n");
	fprint(fd, "<ADDRESS>bijou/1.0 server at %s port %s</ADDRESS>\n", hostname, port);
	fprint(fd, "</BODY></HTML>\n");

	return;
}

/* HTTP 404 not found */

void http_404(int fd, char *path, char *hostname, char *port, struct request req) {

	char date[32];

	rfc1123_time(date, time(0));

	fprint(fd, "HTTP/1.1 404 Not Found\n");
	fprint(fd, "Date: %s\n", date );
	fprint(fd, "Server: bijou/1.0 (Plan 9)\n");
	fprint(fd, "Content-Type: text/html\n");

	fprint(fd, "\r\n");

	/* if the method was HEAD, return after printing the document header */

	if ( strcmp(req.method, "HEAD") == 0 ) {
		return;
	}

	fprint(fd, "<HTML><HEAD>\n");
	fprint(fd, "<TITLE>404 Not Found</TITLE>\n");
	fprint(fd, "</HEAD><BODY>\n");
	fprint(fd, "<H1>Not Found</H1>\n");
	fprint(fd, "The requested URL %s was not found on this server.<P>\n", path);
	fprint(fd, "<HR>\n");
	fprint(fd, "<ADDRESS>bijou/1.0 server at %s port %s</ADDRESS>\n", hostname, port);
	fprint(fd, "</BODY></HTML>\n");

	return;
}

/* HTTP 500 internal server error */

void http_500(int fd, char *hostname, char *port, struct request req) {

	char date[32];

	rfc1123_time(date, time(0));

	fprint(fd, "HTTP/1.1 500 Internal Server Error\n");
	fprint(fd, "Date: %s\n", date );
	fprint(fd, "Server: bijou/1.0 (Plan 9)\n");
	fprint(fd, "Content-Type: text/html\n");

	fprint(fd, "\r\n");

	/* if the method was HEAD, return after printing the document header */

	if ( strcmp(req.method, "HEAD") == 0 ) {
		return;
	}

	fprint(fd, "<HTML><HEAD>\n");
	fprint(fd, "<TITLE>500 Internal Server Error</TITLE>\n");
	fprint(fd, "</HEAD><BODY>\n");
	fprint(fd, "<H1>Internal Server Error</H1>\n");
	fprint(fd, "The server encountered an internal error or misconfiguration and was unable to complete your request.<P>\n");
	fprint(fd, "Please contact the server administrator and inform them of the time the error occurred, and anything you might have done that may have caused the error.<P>\n");
	fprint(fd, "More information about this error may be available in the server error log.<P>\n");
	fprint(fd, "<HR>\n");
	fprint(fd, "<ADDRESS>bijou/1.0 server at %s port %s</ADDRESS>\n", hostname, port);
	fprint(fd, "</BODY></HTML>\n");

	return;
}

/* HTTP 501 method not implemented */

void http_501(int fd, char *hostname, char *port, struct request req) {

	char date[32];

	rfc1123_time(date, time(0));

	fprint(fd, "HTTP/1.1 501 Method Not Implemented\n");
	fprint(fd, "Date: %s\n", date );
	fprint(fd, "Server: bijou/1.0 (Plan 9)\n");
	fprint(fd, "Allow: GET\n");
	fprint(fd, "Content-Type: text/html\n");

	fprint(fd, "\r\n");

	/* if the method was HEAD, return after printing the document header */

	if ( strcmp(req.method, "HEAD") == 0 ) {
		return;
	}

	fprint(fd, "<HTML><HEAD>\n");
	fprint(fd, "<TITLE>501 Method Not Implemented</TITLE>\n");
	fprint(fd, "</HEAD><BODY>\n");
	fprint(fd, "<H1>Method Not Implemented</H1>\n");
	fprint(fd, "%s to %s not supported.<P>\n", req.method, req.uri);

	if ( strcmp(req.version, "") == 0 ) {
		fprint(fd, "Invalid method in request %s %s<P>\n", req.method, req.uri);
	}

	else {
		fprint(fd, "Invalid method in request %s %s %s<P>\n", req.method, req.uri, req.version);
	}

	fprint(fd, "<HR>\n");
	fprint(fd, "<ADDRESS>bijou/1.0 server at %s port %s</ADDRESS>\n", hostname, port);
	fprint(fd, "</BODY></HTML>\n");

	return;
}
