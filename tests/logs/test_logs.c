
#include "logs.h"

#if defined(__APPLE__) || defined(__linux__)
#include <unistd.h>
#define SLEEP_MS(ms) usleep(ms*1000)
#elif defined(__WIN32__)
#include "windows.h"
#define SLEEP_MS(ms) Sleep(ms)
#endif

static const char* tag = "TestApp";

void func()
{
    LogI(tag, "Some function started");
    SLEEP_MS(100);
    LogI(tag, "Some function finished");
}

int main(int argc, char** argv)
{
    logsInit();

    (void)argv; // unused

    LogI(tag, "App started with argc:%d", argc);
    LogE(tag, "Just test error log");
    LogD(tag, "Just test debug log");

    func();

    int i;
    int count = 20;
    for (i = 0; i < count; i++) {
        LogD(tag, "Sleeping for 40ms... (%d/%d)", i+1, count);
        SLEEP_MS(60);
    }

    LogI(tag, "App exiting");

    logsDeinit();
    return 0;
}
