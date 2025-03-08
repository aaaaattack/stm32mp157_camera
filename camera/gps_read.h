#ifndef GPS_READ_H
#define GPS_READ_H

extern "C"
{
    #include <stdio.h>
    #include <string.h>
    #include <sys/types.h>
    #include <errno.h>
    #include <sys/stat.h>
    #include <fcntl.h>
    #include <unistd.h>
    #include <termios.h>
    #include <stdlib.h>
}


extern int gps_fd;

#define GPS_DEV_FILE "/dev/ttySTM3"
void gps_init(void);
int set_opt(int, int, int, char, int);
int open_port(char*);
int read_gps_raw_data(int, char*);
int parse_gps_raw_data(char*, char*, char*, char*, char*, char*);

#endif // GPS_READ_H
