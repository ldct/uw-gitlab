#include <parse.h>
#include <io.h>

int tokenize(char* command, int* offsets) {
    int spaces[MAX_ARGUMENTS];

    int num_spaces = 0;

    for (int command_offset = 0; command[command_offset]; command_offset++) {
        if (command[command_offset] == ' ') {
            spaces[num_spaces++] = command_offset;
            command[command_offset] = 0;
        }
    }

    offsets[0] = 0;
    for (int j=0; j<num_spaces; j++) {
        offsets[j+1] = spaces[j] + 1;
    }

    // n spaces => n+1 words
    return num_spaces + 1;
}

void testTokenize() {
    logf("testtokenize\r\n");

    char* command = "hello world";
    int offsets[MAX_ARGUMENTS];

    int numwords = tokenize(command, offsets);

    logf("found %d words\r\n", numwords);
    for (int i=0; i<numwords; i++) {
        logf("there is a word at offset %d: %s\r\n", offsets[i], command + offsets[i]);
    }

    logf("testtokenize done\r\n");
}
