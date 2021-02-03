#include "command_fsm.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define NEXT_STATE(result, new_state) \
do                                    \
{                                     \
    res = (result);                   \
    ctx->state = (new_state);         \
} while (0)

fsm_res_e fsm_init(fsm_context_t *ctx)
{
    ctx->idx = 0;
    memset(ctx->input, '\0', INPUT_LENGTH);
    ctx->state = FSM_STATE_EXT_READ_COMMAND;
    return FSM_RES_CONTINUE;
}

static void clear_buffer(fsm_context_t *ctx)
{
    ctx->idx = 0;
    memset(ctx->input, '\0', INPUT_LENGTH);
}

static fsm_res_e change_state_by_command(fsm_context_t *ctx)
{
    fsm_res_e res;
    if (strcmp(ctx->input, "puts") == 0)
    {
        NEXT_STATE(FSM_RES_CONTINUE, FSM_STATE_EXT_PUTS_COMMAND);
    }
    else if (strcmp(ctx->input, "exit") == 0)
    {
        NEXT_STATE(FSM_RES_DO_INTERNAL, FSM_STATE_INT_EXIT);
    }
    else if (strcmp(ctx->input, "set") == 0)
    {
        NEXT_STATE(FSM_RES_CONTINUE, FSM_STATE_EXT_SET_TYPE);
    }
    else
    {
        NEXT_STATE(FSM_RES_DO_INTERNAL, FSM_STATE_INT_WRONG_COMMAND);
    }

    clear_buffer(ctx);
    return res;
}

static fsm_res_e read_command(char ch, fsm_context_t *ctx)
{
    fsm_res_e res = FSM_RES_CONTINUE;
    if (isspace(ch) == 0)
    {
        ctx->input[ctx->idx++] = ch;
        #if 0
            if (ctx->idx < INPUT_LENGTH - 1)
            {
                ctx->input[ctx->idx++] = ch;
            }
            else
            {
                ctx->idx = 0;
                ctx->res = FSM_RES_DO_INTERNAL;
                ctx->state = FSM_STATE_INT_WRONG_COMMAND;
            }
        #endif
    }
    else
    {
        printf("Read command: %s\n", ctx->input);
        NEXT_STATE(FSM_RES_DO_INTERNAL, FSM_STATE_INT_CHECK_COMMAND);
    }

    return res;
}

static fsm_res_e read_string(char ch, fsm_context_t *ctx)
{
    fsm_res_e res = FSM_RES_CONTINUE;
    if (ch != '\n')
    {
        ctx->input[ctx->idx++] = ch;
    }
    else
    {
        printf("Read string: %s\n", ctx->input);
        NEXT_STATE(FSM_RES_CONTINUE, FSM_STATE_EXT_READ_COMMAND);
        clear_buffer(ctx);
    }

    return res;
}

static fsm_res_e set_type(char ch, fsm_context_t *ctx)
{
    fsm_res_e res = FSM_RES_CONTINUE;
    if (isspace(ch) == 0)
    {
        ctx->input[ctx->idx++] = ch;
    }
    else
    {
        //check correct type
        //save set type
        printf("Read set type: %s\n", ctx->input);
        NEXT_STATE(FSM_RES_CONTINUE, FSM_STATE_EXT_SET_NAME);
        clear_buffer(ctx);
    }

    return res;
}

static fsm_res_e set_name(char ch, fsm_context_t *ctx)
{
    fsm_res_e res = FSM_RES_CONTINUE;
    if (isspace(ch) == 0)
    {
        ctx->input[ctx->idx++] = ch;
    }
    else
    {
        //save set name
        printf("Read set name: %s\n", ctx->input);
        NEXT_STATE(FSM_RES_CONTINUE, FSM_STATE_EXT_SET_VALUE);
        clear_buffer(ctx);
    }

    return res;
}

static fsm_res_e set_value(char ch, fsm_context_t *ctx)
{
    fsm_res_e res = FSM_RES_CONTINUE;
    if (isspace(ch) == 0)
    {
        ctx->input[ctx->idx++] = ch;
    }
    else
    {
        //save value for last name
        printf("Read set value: %s\n", ctx->input);
        NEXT_STATE(FSM_RES_CONTINUE, FSM_STATE_EXT_READ_COMMAND);
        clear_buffer(ctx);
    }

    return res;
}

static fsm_res_e fsm_iter(char ch, fsm_context_t *ctx)
{
    fsm_res_e res;
    switch(ctx->state)
    {
        case FSM_STATE_EXT_READ_COMMAND:
        {
            res = read_command(ch, ctx);
            break;
        }
        case FSM_STATE_EXT_PUTS_COMMAND:
        {
            res = read_string(ch, ctx);
            break;
        }
        case FSM_STATE_EXT_SET_TYPE:
        {
            res = set_type(ch, ctx);
            break;
        }
        case FSM_STATE_EXT_SET_NAME:
        {
            res = set_name(ch, ctx);
            break;
        }
        case FSM_STATE_EXT_SET_VALUE:
        {
            res = set_value(ch, ctx);
            break;
        }
        case FSM_STATE_INT_CHECK_COMMAND:
        {
            res = change_state_by_command(ctx);
            break;
        }
        case FSM_STATE_INT_WRONG_COMMAND:
        {
            printf("Wrong command\n");
            exit(1);
            break;
        }
        case FSM_STATE_INT_EXIT:
        {
            res = FSM_RES_EXIT;
            break;
        }
    }

    return res;
}

fsm_res_e fsm(char ch, fsm_context_t *ctx)
{
    fsm_res_e res;
    do
    {
        res = fsm_iter(ch, ctx);
    } while (res == FSM_RES_DO_INTERNAL);

    return res;
}

/*
    set <type> <name> = <value>;     <type> := int | double | str
    mul <left> <right>
    sum <left> <right>
    puts <str>
    store <filename>
    load <filename>
    exit
*/
