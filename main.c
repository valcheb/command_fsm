#include "command_fsm.h"

#include <stdio.h>

int main()
{
    char input[] = "puts \"this is a test string\"\nsets int a 10\nsts int a 10\nset int a 10\nexit\n";
    int idx = 0;
    fsm_context_t context;
    fsm_res_e res = fsm_init(&context);

    printf("Input start\n-------\n");
    printf("%s", input);
    printf("Input end\n-------\n");
    do
    {
        res = fsm(input[idx++], &context);
    } while (res == FSM_RES_CONTINUE);

    printf("Exit programm\n");

    return 0;
}
