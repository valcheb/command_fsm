#include "command_fsm.h"
#include "union_storage.h"

#include <stdio.h>

#define STORAGE_SIZE 5

int main()
{
    char input[] =
    "set double a 0.5\n"
    "set int b 3\n"
    "sum a b\n"
    "sum unnknown b\n"
    "set int test_long_name 5\n"
    "set_very_long_input_you_see int test 77\n"
    "set int ts_int 77\n"
    "set double ts_db 13.4\n"
    "set double wrong_db 13..\n"
    "set int wrong_int 12.\n"
    "puts \"test_puts\"\n"
    "set_wrong int a 10\n"

    "set int ts_int1 77\n"
    "set int ts_int2 77\n"
    "set int ts_int3 77\n"
    "set int ts_int4 77\n"
    "set int ts_int5 77\n"
    "set int ts_int6 77\n"
    "set int ts_int7 77\n"
    "set int ts_int8 77\n"
    "set int ts_int9 77\n"

    "exit\n";

    us_elem_t elem_array[STORAGE_SIZE];
    union_storage_t storage;
    us_init(&storage, elem_array, STORAGE_SIZE);

    int idx = 0;
    fsm_context_t context;
    fsm_res_e res = fsm_init(&context, &storage);

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
