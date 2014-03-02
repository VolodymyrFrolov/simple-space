
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <unistd.h> // usleep()
#include <stdbool.h>

#include "wrp_mutex.h"
#include "wrp_thread.h"
#include "wrp_cond.h"
#include "wrp_common.h"

wrp_mutex_t the_mutex;
wrp_cond_t the_cond;
wrp_thread_t thread1;
wrp_thread_t thread2;

int g_products = 0;
int g_consumable_min = 3;

wrp_thread_ret_t win_attr some_func(void* arg) {
    int i;
    int counts = 3;
    for (i = 0; i < counts; ++i) {
        wrp_mutex_lock(&the_mutex);
        printf("func %d iteration: %d started\n", (intptr_t)arg, i + 1);
 
        wrp_sleep_ms(1000);

        printf("func %d iteration: %d finished\n", (intptr_t)arg, i + 1);
        wrp_mutex_unlock(&the_mutex);

        wrp_sleep_ms(50);
    }
    return (wrp_thread_ret_t)arg;
}

bool ready_to_consume() {
    return g_products >= g_consumable_min;
}

wrp_thread_ret_t win_attr consumer_func(void* arg) {
    printf("consuming started\n");
    uintptr_t i = (uintptr_t)arg;
    
    while (i > 0) {
        wrp_mutex_lock(&the_mutex);
        while (!ready_to_consume()) {
            int ret = wrp_cond_wait(&the_cond, &the_mutex, 4000);
            if (ret == 0) {
                printf("consuming succeeded\n");
            } else if (ret == WRP_TIMEOUT_ERR) {
                printf("consuming timeout\n");
            } else {
                printf("consuming error\n");
            }
        }
        i--;
        wrp_mutex_unlock(&the_mutex);
    }
    printf("consuming finished\n");
    return 0;
}

wrp_thread_ret_t win_attr producer_func(void* arg) {
    printf("producing started\n");
    while (g_products < (uintptr_t)arg) {
        wrp_mutex_lock(&the_mutex);
        wrp_sleep_ms(1000);
        g_products++;
        wrp_mutex_unlock(&the_mutex);
        printf("produced:%d/%d\n", g_products, (uintptr_t)arg);
        if (ready_to_consume())
            wrp_cond_signal(&the_cond);
    }
    printf("producing finished\n");
    return 0;
}

int main(void) {

    // ==== Test Case 1 ====
    printf("Test Case 1: Started\n");
    wrp_mutex_init(&the_mutex);

    wrp_thread_create(&thread1, some_func, (void*)1);
    wrp_sleep_ms(50);
    wrp_thread_create(&thread2, some_func, (void*)2);

    wrp_thread_ret_t ret1;
    wrp_thread_ret_t ret2;
    wrp_thread_join(thread1, &ret1);
    wrp_thread_join(thread2, &ret2);
    
    wrp_mutex_destroy(&the_mutex);

    printf("main: thread1 returned: %d\n", (uintptr_t)ret1);
    printf("main: thread2 returned: %d\n", (uintptr_t)ret2);
    printf("Test Case 1: Finished\n");
    
    // ==== Test Case 2 ====
    printf("Test Case 2: Started\n");
    wrp_mutex_init(&the_mutex);
    wrp_cond_init(&the_cond);

    wrp_thread_create(&thread1, consumer_func, (void*)3);
    wrp_sleep_ms(50);
    wrp_thread_create(&thread2, producer_func, (void*)3);
    
    wrp_thread_join(thread1, NULL);
    wrp_thread_join(thread2, NULL);
    
    wrp_cond_destroy(&the_cond);
    wrp_mutex_destroy(&the_mutex);
    printf("Test Case 2: Finished\n");
    return EXIT_SUCCESS;
}
