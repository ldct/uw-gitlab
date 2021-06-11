#include <syslib.h>
#include <userprogram.h>
#include <bwio.h>
#include <stdlib.h>
#include <map.h>
#include <nameServer.h>
#include <panic.h>
#include <string.h>

extern int seedSeed;

// deterministic PRNG
unsigned lrand(int* seed) {
	return *seed = (*seed * 1103515245 + 12345) & ((1U << 31) - 1);
}

// gcc is generating memcpys

const char* KYS_MSG = "kys";
const char* SIGNUP_MSG = "_signup";
const char* FIRSTCHOICE_MSG = "first";
const char* WON_MSG = "won";
const char* LOST_MSG = "lost";
const char* DRAW_MSG = "draw";
const char* ROCK_MSG = "rock";
const char* PAPER_MSG = "paper";
const char* SCISSOR_MSG = "scissors";
const char* I_QUIT_MSG = "i_quit";
const char* THEY_QUIT_MSG = "they_quit";

/*
Optimal (Nash Equilibrium) Strategy
*/
const char* randomMove(int* seed) {
    unsigned r = lrand(seed) % 3;
    if (r == 0) return ROCK_MSG;
    if (r == 1) return PAPER_MSG;
    return SCISSOR_MSG;
}

int isMoveStr(const char* msg) {
    if (0 == strcmp(msg, ROCK_MSG)) return 1;
    if (0 == strcmp(msg, PAPER_MSG)) return 1;
    if (0 == strcmp(msg, SCISSOR_MSG)) return 1;

    return 0;
}

int isMoveChar(char msg) {
    if (msg == 'r') return 1;
    if (msg == 'p') return 1;
    if (msg == 's') return 1;
    return 0;
}

void trace_Send(const char* taskName, int tid, const char* msg, char* reply) {
    int ret = Send(tid, msg, strlen(msg) + 1, reply, 100);
    bwprintf(COM2, "[%s %d]\t%d = Send(%d, %s, =%s)\r\n", taskName, MyTid(), ret, tid, msg, reply);
}

void trace_Receive(const char* taskName, int* tid, char* msg) {
    Receive(tid, msg, 100);
    bwprintf(COM2, "[%s %d]\tReceive(=%d, =%s)\r\n", taskName, MyTid(), *tid, msg);
}

void wrap_Reply(int who, const char* msg) {
    Reply(who, msg, strlen(msg) + 1);
}

int whoWon(char p1Move, char p2Move) {
    if (p1Move == p2Move) return 0;
    if (p1Move == 'r' && p2Move == 's') return 1;
    if (p1Move == 'r' && p2Move == 'p') return 2;
    if (p1Move == 'p' && p2Move == 's') return 2;
    if (p1Move == 'p' && p2Move == 'r') return 1;
    if (p1Move == 's' && p2Move == 'p') return 1;
    if (p1Move == 's' && p2Move == 'r') return 2;
    bwprintf(COM2, "whoWon: invalid\r\n");
    for (;;) {}
}

void rpsServer() {
    bwprintf(COM2, "Entering the RPS server\r\n");
    // Queue<int>
    Queue signups;
    char buf[100];
    int who;
    int p1 = -1;
    int p2 = -1;
    char p1Move = 'x';
    char p2Move = 'x';

    bwprintf(COM2, "surely this isnt where it crashed to the ground");

    char yourOpponentHasQuit[64];

    int i;
    for (i=0; i<64; i++) {
	//bwprintf(COM2 , "Seriouisly? %d\r\n", i);
        yourOpponentHasQuit[i] = 0;
    }

    bwprintf(COM2, "Did my task crash here?\r\n");

    initializeQueue("signups", &signups);

    bwprintf(COM2, "Did my queue crash here?\r\n");

    RegisterAs("server");

    bwprintf(COM2, "Did the registration crash everything?\r\n");

    while (1) {
        trace_Receive("rpsServer", &who, buf);

        // handle receive
        if (0 == strcmp(buf, KYS_MSG)) {
            wrap_Reply(who, "ok");
            Exit();
        } else if (yourOpponentHasQuit[who]) {
            wrap_Reply(who, THEY_QUIT_MSG);
        } else if (0 == strcmp(buf, SIGNUP_MSG)) {
            push(&signups, (void*)who);
        } else if (p1 == who && isMoveStr(buf)) {
            p1Move = buf[0];
        } else if (p2 == who && isMoveStr(buf)) {
            p2Move = buf[0];
        } else if (p1 == who && 0 == strcmp(buf, I_QUIT_MSG)) {
            p1Move = 'q';
        } else if (p2 == who && 0 == strcmp(buf, I_QUIT_MSG)) {
            p2Move = 'q';
        } else {
            bwprintf(COM2, "[rpsServer]\tUnknown message %s %d %d\r\n", buf, p1, p2);
        }

        if (ringFill(&signups) > 2) {
            PANIC("signups should not have exceeded 2");;
        } else if (p1 == -1 && 2 == ringFill(&signups)) {
            p1 = (int)pop(&signups);
            p2 = (int)pop(&signups);
            wrap_Reply(p1, FIRSTCHOICE_MSG);
            wrap_Reply(p2, FIRSTCHOICE_MSG);
        } else if (isMoveChar(p1Move) && isMoveChar(p2Move)) {
            int winner = whoWon(p1Move, p2Move);
            p1Move = 'x';
            p2Move = 'x';
            if (winner == 0) {
                wrap_Reply(p1, DRAW_MSG);
                wrap_Reply(p2, DRAW_MSG);
            } else if (winner == 1) {
                wrap_Reply(p1, WON_MSG);
                wrap_Reply(p2, LOST_MSG);
            } else if (winner == 2) {
                wrap_Reply(p1, LOST_MSG);
                wrap_Reply(p2, WON_MSG);
            } else {
                bwprintf(COM2, "unknown \r\n");
                for (;;) {}
            }

        } else if (p1Move == 'q') {
            wrap_Reply(p1, THEY_QUIT_MSG);
            wrap_Reply(p2, THEY_QUIT_MSG);
            yourOpponentHasQuit[p2] = 1;
            p1 = -1;
            p2 = -1;
            p1Move = 'x';
            p2Move = 'x';
        } else if (p2Move == 'q') {
            wrap_Reply(p1, THEY_QUIT_MSG);
            wrap_Reply(p2, THEY_QUIT_MSG);
            yourOpponentHasQuit[p1] = 1;
            p1 = -1;
            p2 = -1;
            p1Move = 'x';
            p2Move = 'x';
        }
    }
}

void runRps(unsigned rounds) {
    char buf[100];
    int seed = seedSeed + MyTid();
    trace_Send("rpsClient", WhoIs("server"), SIGNUP_MSG, buf);
    if (0 == strcmp(buf, THEY_QUIT_MSG)) {
        Exit();
    } else if (0 != strcmp(buf, FIRSTCHOICE_MSG)) { /* this should never happen and has never happened in testing */
        bwprintf(COM2, "rpsClient\tUnexpected, quitting\r\n");
        Exit();
    }
    int i;
    for (i=0; i<rounds; i++) {
        trace_Send("rpsClient", WhoIs("server"), randomMove(&seed), buf);
        if (0 == strcmp(buf, THEY_QUIT_MSG)) {
            Exit();
        }
    }
    trace_Send("rpsClient", WhoIs("server"), I_QUIT_MSG, buf);
    Exit();
}

void rpsClient() {
    int seed = seedSeed + MyTid();
    runRps(lrand(&seed) % 10);
}

void killServer() {
    char buf[100];
    trace_Send("killer", WhoIs("server"), KYS_MSG, buf);
    trace_Send("killer", getNsTid(), KYS_MSG, buf);
}

void k2_rps_main() {
    Create(1, nameServer);
    Create(2, rpsServer);
    int k;
    for (k=0; k<20; k++) {
        Create(k+3, rpsClient);
    }
    Create(100, killServer);
}
