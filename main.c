#include "command_fsm.h"

#include <stdio.h>

int main()
{
    char input[] = "puts this is a test string\nset int a 10\nexit\n";
    int idx = 0;
    fsm_context_t context;
    fsm_init(&context);

    do
    {
        fsm(input[idx++], &context);
    } while (context.res == FSM_RES_CONTINUE);

    printf("Exit programm\n");

    return 0;
}
