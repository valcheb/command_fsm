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
    FSM_STATE_EXT_SKIP,
    FSM_STATE_EXT_PUTS_COMMAND,
    FSM_STATE_EXT_SET_TYPE,
    FSM_STATE_EXT_SET_NAME,
    FSM_STATE_EXT_SET_VALUE,
    FSM_STATE_INT_EXIT,
    FSM_STATE_INT_CHECK_COMMAND,
    FSM_STATE_INT_ERROR
} fsm_state_e;

typedef enum
{
    FSM_ERROR_WRONG_COMMAND = 0,
    FSM_ERROR_TOO_LONG_INPUT,
    FSM_ERROR_WRONG_SET_TYPE,
    FSM_ERROR_INCORRECT_VALUE,
    FSM_ERROR_WRONG_CALC_ARG,
    FSM_ERROR_WRONG_LOAD_FILENAME
} fsm_error_e;

#define INPUT_LENGTH 256
typedef struct
{
    fsm_state_e state;
    fsm_error_e error;
    char        input[INPUT_LENGTH];
    int         idx;
} fsm_context_t;

fsm_res_e fsm_init(fsm_context_t *ctx);
fsm_res_e fsm(char ch, fsm_context_t *ctx);

#endif /*COMMAND_FSM_H_*/
