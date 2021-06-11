#define SVC_MODE 0x93
#define SYS_MODE 0x9F
#define IRQ_MODE 0x92
#define USR_MODE 0b10000

#define SWI_OPCODE_FLAG 0x00ffffff

#define DCACHE_BIT 0x00000004
#define ICACHE_BIT 0x00001000

void disableDCache();
void enableDCache();
void disableICache();
void enableICache();
void hypeTrain();
void derail();
