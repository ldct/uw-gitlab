#include <bwio.h>
#include <ts7200.h>
#include <timer.h>

int* com1_flags = (int *)( UART1_BASE + UART_FLAG_OFFSET );
int* com1_data = (int *)( UART1_BASE + UART_DATA_OFFSET );
int* com2_flags = (int *)( UART2_BASE + UART_FLAG_OFFSET );
int* com2_data = (int *)( UART2_BASE + UART_DATA_OFFSET );

void PANIC(const char* msg) {
    bwprintf(COM2, msg);
}

void aFailed(char *file, int line, const char* msg) {
    bwprintf(COM2, "ASSERTION FAILURE %s at %s %d\r\n", msg, file, line);
}

#define EXPECTED_BYTES_BACK 2

int main( int argc, char* argv[] ) {

    bwsetfifo(COM1, OFF);
    bwsetfifo(COM2, OFF);
    bwsetspeed(COM1, 2400);
    bwsetstopbits(COM1, ON);

    int num_reports = EXPECTED_BYTES_BACK;

    initializeTimer(3, 508000, getWrap(3), 0);


    unsigned now = getValue(3); 

    while (1) {

        if (
            (*com1_flags & RXFF_MASK)
        ) {
            char ch = *com1_data;
            bwprintf(COM2, "{%x}\t", ch);
            num_reports++;
        }

        if (num_reports == EXPECTED_BYTES_BACK) {
            bwprintf(COM1, "%c", 193);
            unsigned next_now = getValue(3);
            bwprintf(COM2, "%u\r\n", (now - next_now) / 508);
            now = next_now;
            num_reports = 0;
        }

    }

    return 0;
}
