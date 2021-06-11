#include <ascii.h>
#include <cli.h>
#include <hud.h>
#include <io.h>
#include <ipc_codes.h>
#include <panic.h>
#include <parse.h>
#include <programs.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslib.h>
#include <shortMove.h>
#include <haltVisit.h>
#include <pathfinding.h>
#include <graph.h>

char lines[SCROLLBACK_LINES][LINE_WIDTH + 10];

int visible_line_start;
int visible_line_end;
int line_end;

void printCliDebugLine() {
    lprintf(
        COM2,
        "\033[s\033[%d;1Hvisible=[%d,%d) end=%d           \033[u",
        CLI_DEBUG_LINENUM,
        visible_line_start,
        visible_line_end,
        line_end
    );
}

/*
(lineno) - scrolled up
[lineno] - autoscrolled or not scrolled
{lineno} - scrolled down
*/
void log(const char* s) {

    const int NUM_LINES = SCROLL_END - SCROLL_START + 1;

    char* line = lines[line_end];

    strncpy(line, s, LINE_WIDTH-1);

    if (strlen(s) > LINE_WIDTH - 1) {
        strncpy(line+LINE_WIDTH-1, "\u23CE", 5);
    }

    if (line_end == 0) {
        visible_line_end++;
        lprintf(COM2, "[%d]\t%s", visible_line_end-1, line);
    } else if (line_end < NUM_LINES) {
        visible_line_end++;
        lprintf(COM2, "\r\n[%d]\t%s", visible_line_end-1, line);
    }

    line_end++;
    if (visible_line_end - visible_line_start > NUM_LINES) {
        visible_line_start++;
    }

    if (line_end == visible_line_end + 1) { // if scroll one displays the most recent, do it
        visible_line_start++;
        visible_line_end++;
        lprintf(COM2, "\033[s\033[%d;1H\033D[%d]\t%s\033[u", SCROLL_END, visible_line_end-1, lines[visible_line_end-1]);
    }

    printCliDebugLine();

    if (strlen(s) > LINE_WIDTH - 1) {
        log(s + LINE_WIDTH - 1);
    }

}

void CLI_sl(const char* user_input, int offsets[MAX_ARGUMENTS]) {

    int path1[] = {
        trackNodeFromName("C7")->id,
        trackNodeFromName("MR18")->id,
        trackNodeFromName("BR5")->id,
        trackNodeFromName("MR7")->id,
        trackNodeFromName("E11")->id,
        trackNodeFromName("D10")->id,
        trackNodeFromName("MR8")->id,
        trackNodeFromName("BR9")->id,
        trackNodeFromName("D5")->id,
        trackNodeFromName("E6")->id,
        trackNodeFromName("BR10")->id,
        trackNodeFromName("D4")->id,
        trackNodeFromName("B6")->id,
        trackNodeFromName("MR13")->id,
        trackNodeFromName("C12")->id,
        trackNodeFromName("MR14")->id,
        trackNodeFromName("A4")->id,
        trackNodeFromName("B16")->id,
        trackNodeFromName("BR15")->id,
        trackNodeFromName("C10")->id,
        -1
    };


    for (int i=0; i<20; i++) {
        logf("%d", i);
    }

    char pathBuf[1000];
    serializePath(pathBuf, path1);
    logf("xxxx(xxxx)=%s", pathBuf);

    serializePath(pathBuf, path1);
    logf("xxxxx(xxxx)=%s", pathBuf);

}

const CliCommand cliCommands[] = {
    {1, "sl", CLI_sl, "print a steam locomotive"},
    {1, "help", help, "display this help text"},
    {3, "add", CLI_add, "adds two numbers"},
    {3, "lm", CLI_lm, "long move - go to a particular destination then stop"},
    {3, "hr", CLI_hr, "halt with reverse - go to a particular destination then stop"},
    {5, "hh", halthalt, "run lm on both trains"},
    {3, "tr", CLI_tr, "set train speed"},
    {2, "sd", CLI_sd, "issue speed=0 once given sensor is triggered"},
    {1, "q", CLI_q, "kill the kernel"},
    {2, "rv", CLI_rv, "reverse the train"},
    {3, "sw", CLI_sw, "set switch direction"},
    {2, "sc", sc, "set switch curved"},
    {2, "ss", ss, "set switch straight"},
    {3, "rt", CLI_rt, "run dijkstra vs floyd warshall test"},
    {3, "sm", CLI_sm, "move train a short distance"},
    {1, "tfr", tfr, "test"},
    {2, "hl", CLI_hl, "loop one train"},
    {2, "ut", CLI_ut, "use track A/B"},
    {2, "ts", CLI_ts, "trigger sensor"}
};
const int NUM_SCRIPTS = sizeof(cliCommands) / sizeof(CliCommand);

void help(const char* user_input, int offsets[MAX_ARGUMENTS]) {
    logf("Commands:");
    for (int i=0; i<NUM_SCRIPTS; i++) {
        logf("%s\t%s", cliCommands[i].name, cliCommands[i].description);
    }
}

void handleUserInput(char* user_input) {

    logf("%s", user_input);

    int offsets[MAX_ARGUMENTS];
    int numwords = tokenize(user_input, offsets);

    int matched = 0;

    for (int i=0; i<NUM_SCRIPTS; i++) {
        if (numwords == cliCommands[i].numargs && 0 == strcmp(user_input, cliCommands[i].name)) {
            ASSERT(!matched, "two commands with the same name");
            cliCommands[i].fp(user_input, offsets);
            matched = 1;
        }
    }


    if (0 == strcmp("", user_input)) {
    } else if (!matched) {
        logf("Unknown command %s(%d)", user_input, numwords);
    }
}

void cli() {

    char user_input[10];
    int user_input_end = 0;

    // canaries: should not be seen
    for (int i=0; i<SCROLLBACK_LINES; i++) {
        lines[i][0] = ':';
        lines[i][1] = '(';
        lines[i][2] = 0;
    }

    visible_line_start = 0;
    visible_line_end = 0;
    line_end = 0;

    lprintf(
        COM2,
        "\033[s\033[%d;1H> _\033[u",
        CLI_INPUT_LINENUM
    );

    for (;;) {
        char ch = Getc(COM2);
        if (ch == '\r' || ch == ASCII_ETX) {
            lprintf(
                COM2,
                "\033[s\033[%d;1H> _            \033[u",
                CLI_INPUT_LINENUM
            );
            user_input[user_input_end++] = 0;
            user_input_end = 0;
            if (ch == '\r') {
                handleUserInput(user_input);
            }
        } else if (ch == ASCII_BACKSPACE) {
            if (user_input_end > 0) {
                user_input_end--;
                lprintf(
                    COM2,
                    "\033[s\033[%d;%dH_ \033[u",
                    CLI_INPUT_LINENUM,
                    user_input_end+3
                );
            }
        } else if (ch == '+') { // scroll down
            if (visible_line_end < line_end) {
                visible_line_start++;
                visible_line_end++;
                lprintf(COM2, "\033[s\033[%d;1H\033D{%d}\t%s\033[u", SCROLL_END, visible_line_end-1, lines[visible_line_end-1]);
            }
        } else if (ch == '-') { // scroll up
            if (visible_line_start > 0) {
                visible_line_start--;
                visible_line_end--;
                lprintf(COM2, "\033[s\033[%d;1H\033M(%d)\t%s\033[u", SCROLL_START, visible_line_start, lines[visible_line_start]);
            }
        } else {
            lprintf(
                COM2,
                "\033[s\033[%d;%dH%c_\033[u",
                CLI_INPUT_LINENUM,
                user_input_end+3,
                ch
            );
            user_input[user_input_end++] = ch;
        }
        printCliDebugLine();
    }
}
