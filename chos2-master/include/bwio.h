#ifndef BWIO_H
#define BWIO_H

#include <stdarg.h>

#define COM1	0
#define COM2	1

#define ON	1
#define	OFF	0

int bwsetfifo( int channel, int state );
int bwsetstopbits(int channel, int two_stop_bits);

int bwsetspeed( int channel, int speed );

int bwputc( int channel, char c );

int bwgetc( int channel );

int bwputx( int channel, char c );

int bwputstr( int channel, char *str );

int bwputr( int channel, unsigned int reg );

void bwputw( int channel, int n, char fc, char *bf );

void bwprintf( int channel, const char *format, ... );

char bwa2i( char ch, char const **src, int base, int *nump );

void bwui2a( unsigned int num, unsigned int base, char *bf );

void bwi2a( int num, char *bf );

#endif
