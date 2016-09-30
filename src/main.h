/* bijou HTTP server for plan 9 by sean caron (scaron@umich.edu) */

#include <u.h>
#include <libc.h>
#include <stdio.h>
#include <ctype.h>
#include <authsrv.h>
#include <mp.h>
#include <libsec.h>

/* whitespace padding to help with formatting directory listings */

#define	TABLE_PAD	"                                        "


/* server configuration record */

struct configuration {

	char *hostname;
	char *port;

	char *icondir;
	char *rootdir;

	int n_indices;
	char **indices;

	char *errlog;
	char *reqlog;

	char *userchar;
	char *userdir;
	char *userdocs;

	int n_translate;
	char **translate;

	char *htpassfile;
};

/* HTTP request */

struct request {

	char *method;
	char *uri;
	char *version;

	char *host;
	char *useragent;

	char *authtype;
	char *authorization;
};

/* htpasswd file data */

struct htpasswd {

	char *realm;

	int n_users;

	char **user;
	char **pass;
};

int main(int argc, char **argv);

void timestamp(char *ptr);
int read_line(int infd, char **ptr);

int parse_request_line(char *line, struct request *req);

void http_301(int fd, char *hostname, char *port, struct request req);

void http_400_following(int fd, char *hostname, char *port, struct request req);
void http_400_invalid(int fd, char *hostname, char *port, struct request req);
void http_401(int fd, char *hostname, char *port, char *realm, struct request req);
void http_403(int fd, char *hostname, char *port, struct request req);
void http_404(int fd, char *path, char *hostname, char *port, struct request req);

void http_500(int fd, char *hostname, char *port, struct request req);
void http_501(int fd, char *hostname, char *port, struct request req);

int serve_file(char *path, Dir *dir, int fd, struct request req);
int serve_dir(char *path, char *uri, char *hostname, char *port, char *icondir, Dir *dir, int fd, struct request req, struct configuration cfg);

void rfc1123_time(char *ptr, ulong tm);
void file_time(char *ptr, ulong dtime);

int read_config_file(char *path, struct configuration *cfg);

void free_config_vars(struct configuration *cfg);
void free_request_vars(struct request *req);

void initialize_request_struct(struct request *req);

int check_directory_path(char *path);
int check_request_version(char *version);

int base64_encode(char *ip, char **op);
int base64_decode(char *ip, char **op);

int uri_encode(char *ip, char **op);
int uri_decode(char *ip, char **op);

int read_htpasswd_file(char *path, struct htpasswd *htpass);
void free_htpasswd_vars(struct htpasswd *htpass);
int check_authorization(char *path, char *rpath, struct configuration cfg, struct request req, struct htpasswd *htpass);

void generate_path(char **path, char **rpath, struct request *req, struct configuration *cfg);
