#include <io.h>
#include <bwio.h>
#include <panic.h>
#include <uiLockServer.h>
#include <ipc_codes.h>
#include <syslib.h>
#include <nameServer.h>
#include <stdio.h>
#include <string.h>

int com1TxS;
int com2TxS;

int Putc(int channel, char c) {
    int serverTid = 0;
    if (channel == COM1) {
		if (com1TxS == -1) com1TxS = WhoIs("com1TxS");
        serverTid = com1TxS;
    } else if (channel == COM2) {
		if (com2TxS == -1) com2TxS = WhoIs("com2TxS");
        serverTid = com2TxS;
    } else {
        PANIC("Putc: unknown channel");
    }
    char sendReq[2];
    sendReq[0] = 'S';
    sendReq[1] = c;
    return Send(serverTid, sendReq, 2, 0x0, 0);
}

int Putc2(int channel, char c1, char c2) {
    int serverTid = 0;
	ASSERT(channel == COM1, "cannot call Putc2 on COM2");
	serverTid = WhoIs("com1TxS");

    char sendReq[3];
    sendReq[0] = UART1_ATOMIC_SEND;
    sendReq[1] = c1;
	sendReq[2] = c2;
    return Send(serverTid, sendReq, 3, 0x0, 0);
}

int Putc2NB(int channel, char c1, char c2) {
    int serverTid = 0;
	ASSERT(channel == COM1, "cannot call Putc2 on COM2");
	serverTid = WhoIs("com1TxS");

    char sendReq[3];
    sendReq[0] = UART1_ATOMIC_NB_SEND;
    sendReq[1] = c1;
	sendReq[2] = c2;
    return Send(serverTid, sendReq, 3, 0x0, 0);
}

int PutcNB(int channel, char c) {
	int serverTid = 0;
    if (channel == COM1) {
        serverTid = WhoIs("com1TxS");
    } else if (channel == COM2) {
        serverTid = WhoIs("com2TxS");;
    } else {
        PANIC("Putc: unknown channel");
    }
    char replyBuf[10];
    char sendReq[2];
    sendReq[0] = 'N';
    sendReq[1] = c;
    return Send(serverTid, sendReq, 2, replyBuf, 10);
}

char Getc(int channel) {
    int serverTid = 0;
    if (channel == COM1) {
        serverTid = WhoIs("com1RxS");
    } else if (channel == COM2) {
        serverTid = WhoIs("com2RxS");
    } else {
        PANIC("Getc: unknown channel");
    }
    char replyBuf[10];
    char sendReq[1];
    sendReq[0] = UART_GETC;
    Send(serverTid, sendReq, 2, replyBuf, 10);
    return replyBuf[0];
}

void Puts(int channel, const char* s) {
    for (const char* c = s; *c; c++) {
        Putc(channel, *c);
    }
}

void xputw( int channel, int n, char fc, char *bf ) {
	char ch;
	char *p = bf;

	while( *p++ && n > 0 ) n--;
	while( n-- > 0 ) Putc( channel, fc );
	while( ( ch = *bf++ ) ) Putc( channel, ch );
}

void xformat(int channel, const char *fmt, va_list va) {
	char bf[12];
	char ch, lz;
	int w;

	while ( ( ch = *(fmt++) ) ) {
		if ( ch != '%' )
			Putc(channel, ch);
		else {
			lz = 0; w = 0;
			ch = *(fmt++);
			switch ( ch ) {
			case '0':
				lz = 1; ch = *(fmt++);
				break;
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				ch = bwa2i( ch, &fmt, 10, &w );
				break;
			}
			switch( ch ) {
			case 0: return;
			case 'c':
				Putc( channel, va_arg( va, int ) );
				break;
			case 's':
				xputw( channel, w, 0, va_arg( va, char* ) );
				break;
			case 'u':
				bwui2a( va_arg( va, unsigned int ), 10, bf );
				xputw( channel, w, lz, bf );
				break;
			case 'd':
				bwi2a( va_arg( va, int ), bf );
				xputw( channel, w, lz, bf );
				break;
			case 'x':
				bwui2a( va_arg( va, unsigned int ), 16, bf );
				xputw( channel, w, lz, bf );
				break;
			case '%':
				bwputc( channel, ch );
				break;
			}
		}
	}
}

/*
Unlocked printf (interrupt-based)
*/
void uprintf( int channel, char *fmt, ... ) {
	va_list va;

	va_start(va,fmt);
	xformat( channel, fmt, va );
	va_end(va);
}

void lprintf(int channel, char *fmt, ...) {
	va_list va;

	va_start(va,fmt);
    acquireUiLock();
	xformat(channel, fmt, va);
    releaseUiLock();
	va_end(va);
}

void logf(char* fmt, ...) {
	va_list va;
	va_start(va,fmt);

	char line[LOGF_MAX_BUFFER];

    vsnprintf(line, LOGF_MAX_BUFFER, fmt, va);

	if (strlen(line) + 1 >= LOGF_MAX_BUFFER) {
		log("warn: long line truncated by logf");
	}
	log(line);
	va_end(va);
}
