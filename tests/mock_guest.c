/*
 * Copyright © 2012 Canonical Ltd. All rights reserved.
 *
 * Author(s): David Barth <david.barth@canonical.com>
 *
 */

#include <pwd.h>
#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>

static struct passwd guest = { "guest",
							   "password",
							   500, 500,
							   "M. Guest",
							   "/tmp",
							   "/bin/true" };
struct passwd *
getpwnam (const char __attribute__((unused)) *username)
{ return &guest; }

int
setgroups(size_t __attribute__((unused)) size,
          const gid_t __attribute__((unused)) *list)
{
	errno = EPERM;
	return -1;
}

int
setgid(gid_t __attribute__((unused)) gid)
{ return 0; }

int
setuid(uid_t __attribute__((unused)) uid)
{ return 0; }

int
setegid(gid_t __attribute__((unused)) gid)
{ return 0; }

int
seteuid(uid_t __attribute__((unused)) uid)
{ return 0; }

int chmod(const char __attribute__((unused)) *path,
          mode_t __attribute__((unused)) mode)
{ return 0; }

int chown(const char __attribute__((unused)) *path,
          uid_t __attribute__((unused)) owner,
          gid_t __attribute__((unused)) group)
{ return 0; }

int execvp(const char __attribute__((unused)) *file,
           char __attribute__((unused)) *const argv[])
{
	return 0;
}
/* wrap _exit, to make sure the gcov_exit function installed with atexit()
   is really called to collect coverage statistics */
void _exit (int exitcode)
{
	exit (exitcode);
}


#define BUFFER_SIZE  512

int
socket_sucker ()
{
	int socket_fd = 0;
	int servlen = 0;
	struct sockaddr_un serv_addr;

	bzero((char *)&serv_addr, sizeof(serv_addr));

	const char * home = getenv("HOME");
	if (home == NULL) {
		return -1;
	}

	serv_addr.sun_family = AF_UNIX;

	unsigned long int printsize = (unsigned long int)snprintf(serv_addr.sun_path, sizeof(serv_addr.sun_path) - 1, "%s/%s", home, ".freerdp-socket");
	if (printsize > sizeof(serv_addr.sun_path) - 1 || printsize < 0) {
		return -1;
	}

	servlen = strlen(serv_addr.sun_path) + sizeof(serv_addr.sun_family);

	if ((socket_fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
		return -1;
	}

	if (connect(socket_fd, (struct sockaddr *)&serv_addr, servlen) < 0) {
		return -1;
	}

	char buffer[BUFFER_SIZE + 2];
	int in = 0;
	int out = 0;

	in = read(socket_fd, buffer, BUFFER_SIZE);

	if (in > 0) {
		out = write(1, buffer, in);
	}

	close(socket_fd);

	if (in > 0 && out > 0 && in == out) {
		return 0;
	} else {
		return -1;
	}
}
