extern int nsTid;
extern int kernelRunning;
extern int com1TxS;
extern int com2TxS;

void initGlobals() {
    nsTid = -1;
    kernelRunning = 1;
    com1TxS = -1;
    com2TxS = -1;
}
