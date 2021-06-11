#include <map.h>
#include <charay.h>
#include <syslib.h>
#include <stdlib.h>
#include <bwio.h>
#include <panic.h>
#include <ipc_codes.h>
#include <string.h>
#include <stdio.h>

#define MAX_REQUEST 100

void RegistrationPreamble(Map* NameTable, char* symbol, int caller){
    int returnCode = putMap(NameTable, alphaNumericHash(symbol), (void*)caller);
    ASSERT(returnCode == 0 || returnCode == 1, "Registration failed");
    Reply0(caller);
}


void RetrievalPreamble(Map* NameTable, char* symbol, int caller){
    int returnCode = (int) getMap(NameTable, alphaNumericHash(symbol));
    if (!returnCode) {
        // todo: make panic the default
        v_Reply_i(caller, -1);
    } else {
        v_Reply_i(caller, returnCode);
    }
}

extern int nsTid;

void nameServer(){
    Map NameTable;
    initializeMap(&NameTable);
    int caller;
    char requestBuf[MAX_REQUEST];
    char command;
    char* symbol;

    nsTid = MyTid();
    while (1) {
        Receive(&caller, requestBuf, MAX_REQUEST);
        command = requestBuf[0];
        symbol = requestBuf;
        symbol += 2;
        if (command == NS_REGISTER) {
            RegistrationPreamble(&NameTable, symbol, caller);
        } else if(command == NS_WHOIS) {
            RetrievalPreamble(&NameTable, symbol, caller);
        } else {
            PANIC("NS: unknown IPC message");
        }
    }
}

int getNsTid() {
    ASSERT(nsTid > 0, "nameserver not up yet");
    return nsTid;
}

void RegisterAs(const char *name) {
    char buffer[100];
    snprintf(buffer, 100, "c %s", name);
    buffer[0] = NS_REGISTER;
    Send(getNsTid(), buffer, strlen(buffer) + 1, 0x0, 0);
}

int WhoIs(const char* name) {
    char buffer[100];
    char receiveBuffer[100];
    snprintf(buffer, 100, "c %s", name);
    buffer[0] = NS_WHOIS;
    int result = Send(getNsTid(), buffer, strlen(buffer) + 1, receiveBuffer, 100);
    ASSERT(result > 0, "nameserver did not reply with anything");
    return *(int*)receiveBuffer;
}
