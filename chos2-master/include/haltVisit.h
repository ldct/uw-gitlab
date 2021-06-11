#ifndef HALTVISIT_H
#define HALTVISIT_H

void doHaltAt(int trainNum, char* whereTo);
void asyncHaltAt(int trainNum, const char* whereTo);
void asyncHaltR(int trainNum, const char* whereTo);

void doHaltR(int trainNum, const char* whereTo, int silence);
void CLI_hl(const char* user_input, int offsets[MAX_ARGUMENTS]);

#endif
