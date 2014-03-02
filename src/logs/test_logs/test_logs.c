#include <stdio.h>
#include "logs.h"

const char* tag = "TestApp";

void func() {
    LOG_ENTER_FUNC(tag);
    LOGD(tag, "I did some action");
    LOG_EXIT_FUNC(tag);
}

int main() {
    init_logs();
    init_write_logs_to_file("test_logs.log");
    LOG_ENTER_FUNC(tag);

    LOGE(tag, "this is error log");
    LOGD(tag, "this is debug log; var=%d", 35);
    LOGV(tag, "this is verbose log");

    func();

    LOG_EXIT_FUNC(tag);
    deinit_write_logs_to_file();
    deinit_logs();

    return 0;
}

