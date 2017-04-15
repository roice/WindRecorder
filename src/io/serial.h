#ifndef SERIAL_H
#define SERIAL_H

#include <string>
#include <vector>

/* serial.cxx */
int serial_open(const char* port);
bool serial_setup(int fd, int baud);
bool serial_write(int, char*, int);
int serial_read(int, char*, int);
void serial_close(int fd);

#endif
