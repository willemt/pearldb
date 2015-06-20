#include <stdlib.h>
#include <errno.h>

/* for fopen */
#include <stdio.h>

/* for strerror */
#include <string.h>

/* for getpid */
#include <unistd.h>

#include "pidfile.h"

void pidfile_write(const char* pid_file_name)
{
    FILE *fp = fopen(pid_file_name, "wt");
    if (!fp)
    {
        fprintf(stderr, "failed to open pid file:%s:%s\n",
                pid_file_name, strerror(errno));
        abort();
    }
    fprintf(fp, "%d\n", (int)getpid());
    fclose(fp);
}
