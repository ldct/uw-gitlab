#ifndef CLI_H
#define CLI_H

#include <parse.h>

#define SCROLLBACK_LINES 50000
#define LINE_WIDTH 80

typedef struct _CliCommand {
    int numargs;
    const char* name;
    void (*fp)(const char* user_input, int offsets[MAX_ARGUMENTS]);
    const char* description;
} CliCommand;

void cli();

#endif
