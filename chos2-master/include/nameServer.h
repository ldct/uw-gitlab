#ifndef NAMESERVER
#define NAMESERVER

void nameServer();

void RegisterAs(const char *name);

int WhoIs(const char *name);

int getNsTid();

#endif
