/* bijou HTTP server for plan 9 by sean caron (scaron@umich.edu) */

#include "main.h"

/* free memory used by configuration structure variables */
void free_config_vars(struct configuration *cfg) {
    int i;

    free(cfg->hostname);
    free(cfg->port);

    free(cfg->icondir);
    free(cfg->rootdir);

    free(cfg->errlog);
    free(cfg->reqlog);

    free(cfg->userchar);
    free(cfg->userdir);
    free(cfg->userdocs);

    // free each index string
    for (i = 0; i < cfg->n_indices; i++) {
        free( *(cfg->indices+i) );
    }

    // free the array of indices
    free(cfg->indices);

    // free each translation string
    for ( i = 0; i < cfg->n_translate; i++ ) {
        free ( *(cfg->translate+i) );
    }

    // free the array of translations
    free(cfg->translate);

    return;
}

/* free memory used by request structure variables */
void free_request_vars(struct request *req) {
    free(req->method);
    free(req->uri);
    free(req->version);

    free(req->host);
    free(req->useragent);

    free(req->authtype);
    free(req->authorization);

    return;
}

void free_htpasswd_vars(struct htpasswd *htpass) {
    int i;

    free(htpass->realm);

    for (i = 0; i < htpass->n_users; i++) {
        free(*(htpass->user+i) );
        free(*(htpass->pass+i) );
    }

    free(htpass->user);

    free(htpass->pass);

    return;
}
