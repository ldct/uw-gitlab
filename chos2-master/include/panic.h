void __attribute__((noreturn)) PANIC(const char* s);

#define ASSERT(expr, msg) \
    if (expr) \
        {} \
    else \
        aFailed(__FILE__, __LINE__, msg)

void aFailed(char *file, int line, const char* msg);
