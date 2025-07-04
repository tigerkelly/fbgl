
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>

#include "fbgl.h"

FILE *_logFd = NULL;

void fbglLogInit(char *logName) {

	_logFd = fopen(logName, "a+");
	if (_logFd == NULL) {
		printf("Failed to create log file. %s\n", strerror(errno));
	}
}

void fbglLogIt(char *msg, ...) {
	va_list valist;

	if (_logFd == NULL) {
		fbglLogInit(LOG_NAME);
	}

    va_start(valist, msg);

    vfprintf(_logFd, msg, valist);
}
