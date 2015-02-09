
#include "logs.h"
#include <stdio.h>

const char* tag = "TestApp";

void func()
{
    LOGI(tag, "I did some action");
}

int main()
{
    INIT_LOGS();

    LOGE(tag, "this is error log");
    LOGI(tag, "this is info  log; var=%d", 35);
    LOGD(tag, "this is debug log");

    func();

    DEINIT_LOGS();

    return 0;
}
