#ifndef IO_H
#define IO_H

#include <stdarg.h>

#define COM1	0
#define COM2	1

int Putc(int channel, char c);
char Getc(int channel);
void Puts(int channel, const char* s);

int PutcNB(int channel, char c);
int Putc2(int channel, char c1, char c2);
int Putc2NB(int channel, char c1, char c2);

void uprintf( int channel, char *fmt, ... );

// defined in ui.c
void lprintf(int channel, char *fmt, ...);

// defined in cli.c
void log(const char* s);

#define LOGF_MAX_BUFFER 200
void logf(char* fmt, ...);

#endif
