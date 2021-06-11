#ifndef PROGRAMS_H
#define PROGRAMS_H

#include <parse.h>

void help(const char* user_input, int offsets[MAX_ARGUMENTS]);

void CLI_add(const char* user_input, int offsets[MAX_ARGUMENTS]);

void CLI_rt(const char* user_input, int offsets[MAX_ARGUMENTS]);

void CLI_rv(const char* user_input, int offsets[MAX_ARGUMENTS]);

void sc(const char* user_input, int offsets[MAX_ARGUMENTS]);

void ss(const char* user_input, int offsets[MAX_ARGUMENTS]);

void halthalt(const char* user_input, int offsets[MAX_ARGUMENTS]);

void CLI_lm(const char* user_input, int offsets[MAX_ARGUMENTS]);

void CLI_hr(const char* user_input, int offsets[MAX_ARGUMENTS]);

void CLI_tr(const char* user_input, int offsets[MAX_ARGUMENTS]);

void CLI_sd(const char* user_input, int offsets[MAX_ARGUMENTS]);

void CLI_q(const char* user_input, int offsets[MAX_ARGUMENTS]);

void CLI_ut(const char* user_input, int offsets[MAX_ARGUMENTS]);

void CLI_sw(const char* user_input, int offsets[MAX_ARGUMENTS]);

void tfr(const char* user_input, int offsets[MAX_ARGUMENTS]);

void CLI_ts(const char* user_input, int offsets[MAX_ARGUMENTS]);

#endif
