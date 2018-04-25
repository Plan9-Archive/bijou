/* bijou HTTP server for plan 9 by sean caron (scaron@umich.edu) */

#include "main.h"

/* Initialize request structure */
void initialize_request_struct(struct request *req) {
    req->method = malloc(sizeof(char));

    if (req->method == 0) {
        exits("MALLOC");
    }

    strcpy(req->method, "");

    req->uri = malloc(sizeof(char));

    if (req->uri == 0) {
        exits("MALLOC");
    }

    strcpy(req->uri, "");

    req->version = malloc(sizeof(char));

    if (req->version == 0) {
        exits("MALLOC");
    }

    strcpy(req->version, "");

    req->host = malloc(sizeof(char));

    if (req->host == 0) {
        exits("MALLOC");
    }

    strcpy(req->host, "");

    req->useragent = malloc(sizeof(char));

    if (req->useragent == 0) {
        exits("MALLOC");
    }

    strcpy(req->useragent, "");

    req->authtype = malloc(sizeof(char));

    if (req->authtype == 0) {
        exits("MALLOC");
    }

    strcpy(req->authtype, "");

    req->authorization = malloc(sizeof(char));

    if (req->authorization == 0) {
        exits("MALLOC");
    }

    strcpy(req->authorization, "");

    return;
}
