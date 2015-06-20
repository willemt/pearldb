#ifndef PIDFILE_H
#define PIDFILE_H

/** Create a pid file containing this process' pid */
void pidfile_write(const char* pid_file_name);

#endif /* PIDFILE_H */
