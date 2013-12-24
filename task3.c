#include <stdio.h>
#include <stdlib.h>
#include <ucontext.h>
#include <unistd.h>
#include <signal.h>

ucontext_t manager, thread1, thread2;
int counter;

void worker(ucontext_t *my_context, ucontext_t *other_context, int number)
{
    while (1) {
        printf("%d\n", number);
        swapcontext(my_context, other_context);
    }
}

void thread_manager()
{
    swapcontext(&manager, &thread1);
    if (counter) {
        swapcontext(&manager, &thread2);
    }
    counter = counter ^ 1;
    alarm(2);
}

int main(void)
{
    char my_stack1[SIGSTKSZ], my_stack2[SIGSTKSZ];
    thread1.uc_stack.ss_sp = my_stack1;
    thread2.uc_stack.ss_sp = my_stack2;
    thread1.uc_stack.ss_size = sizeof(my_stack1);
    thread2.uc_stack.ss_size = sizeof(my_stack2);
    getcontext(&thread1);
    makecontext(&thread1, (void (*)(void)) worker, 3, &thread1, &manager, 1);
    getcontext(&thread2);
    makecontext(&thread2, (void (*)(void)) worker, 3, &thread2, &manager, 2);
    signal(SIGALRM, thread_manager);
    alarm(2);
    while (1);
    return 0;
}

