#ifndef COMMAND_FSM_H_
#define COMMAND_FSM_H_

typedef enum
{
    FSM_RES_CONTINUE = 0,
    FSM_RES_DO_INTERNAL,
    FSM_RES_EXIT
} fsm_res_e;

typedef enum
{
    FSM_STATE_EXT_READ_COMMAND = 0,
    FSM_STATE_EXT_PUTS_COMMAND,
    FSM_STATE_EXT_SET_TYPE,
    FSM_STATE_EXT_SET_NAME,
    FSM_STATE_EXT_SET_VALUE,
    FSM_STATE_INT_EXIT,
    FSM_STATE_INT_CHECK_COMMAND,
    FSM_STATE_INT_WRONG_COMMAND
} fsm_state_e;

#define INPUT_LENGTH 256
typedef struct
{
    fsm_state_e state;
    char        input[INPUT_LENGTH];
    int         idx;
} fsm_context_t;

fsm_res_e fsm_init(fsm_context_t *ctx);
fsm_res_e fsm(char ch, fsm_context_t *ctx);

#endif /*COMMAND_FSM_H_*/
