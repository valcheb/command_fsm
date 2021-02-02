#include "command_fsm.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

void fsm_init(fsm_context_t *ctx)
{
    ctx->idx = 0;
    memset(ctx->input, '\0', INPUT_LENGTH);
    ctx->res = FSM_RES_CONTINUE;
    ctx->state = FSM_STATE_EXT_READ_COMMAND;
}

static void change_state_by_command(fsm_context_t *ctx)
{
    if (strcmp(ctx->input, "puts") == 0)
    {
        ctx->state = FSM_STATE_EXT_PUTS_COMMAND;
        ctx->res = FSM_RES_CONTINUE;
    }
    else if (strcmp(ctx->input, "exit") == 0)
    {
        ctx->state = FSM_STATE_INT_EXIT;
        ctx->res = FSM_RES_DO_INTERNAL;
    }
    else if (strcmp(ctx->input, "set") == 0)
    {
        ctx->state = FSM_STATE_EXT_SET_TYPE;
        ctx->res = FSM_RES_CONTINUE;
    }
    else
    {
        ctx->state = FSM_STATE_INT_WRONG_COMMAND;
        ctx->res = FSM_RES_DO_INTERNAL;
    }

    memset(ctx->input, '\0', INPUT_LENGTH);
}

static void read_command(char ch, fsm_context_t *ctx)
{
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
        ctx->input[ctx->idx] = '\0';
        ctx->idx = 0;
        ctx->res = FSM_RES_DO_INTERNAL;
        ctx->state = FSM_STATE_INT_CHECK_COMMAND;
    }
}

static void read_string(char ch, fsm_context_t *ctx)
{
    if (isspace(ch) == 0)
    {
        ctx->input[ctx->idx++] = ch;
    }
    else
    {
        ctx->input[ctx->idx] = '\0';
        ctx->idx = 0;
        //save string
        printf("Read string: %s\n", ctx->input);
        ctx->state = FSM_STATE_EXT_READ_COMMAND;
        ctx->res = FSM_RES_CONTINUE;
        memset(ctx->input, '\0', INPUT_LENGTH);
    }
}

static void set_type(char ch, fsm_context_t *ctx)
{
    if (isspace(ch) == 0)
    {
        ctx->input[ctx->idx++] = ch;
    }
    else
    {
        ctx->input[ctx->idx] = '\0';
        ctx->idx = 0;
        //check correct type
        //save set type
        printf("Read set type: %s\n", ctx->input);
        ctx->state = FSM_STATE_EXT_SET_NAME;
        ctx->res = FSM_RES_CONTINUE;
        memset(ctx->input, '\0', INPUT_LENGTH);
    }
}

static void set_name(char ch, fsm_context_t *ctx)
{
    if (isspace(ch) == 0)
    {
        ctx->input[ctx->idx++] = ch;
    }
    else
    {
        ctx->input[ctx->idx] = '\0';
        ctx->idx = 0;
        //save set name
        printf("Read set name: %s\n", ctx->input);
        ctx->state = FSM_STATE_EXT_SET_VALUE;
        ctx->res = FSM_RES_CONTINUE;
        memset(ctx->input, '\0', INPUT_LENGTH);
    }
}

static void set_value(char ch, fsm_context_t *ctx)
{
    if (isspace(ch) == 0)
    {
        ctx->input[ctx->idx++] = ch;
    }
    else
    {
        ctx->input[ctx->idx] = '\0';
        ctx->idx = 0;
        //save value for last name
        printf("Read set value: %s\n", ctx->input);
        ctx->state = FSM_STATE_EXT_READ_COMMAND;
        ctx->res = FSM_RES_CONTINUE;
        memset(ctx->input, '\0', INPUT_LENGTH);
    }
}

static void fsm_iter(char ch, fsm_context_t *ctx)
{
    switch(ctx->state)
    {
        case FSM_STATE_EXT_READ_COMMAND:
        {
            read_command(ch, ctx);
            break;
        }
        case FSM_STATE_EXT_PUTS_COMMAND:
        {
            read_string(ch, ctx);
            break;
        }
        case FSM_STATE_EXT_SET_TYPE:
        {
            set_type(ch, ctx);
            break;
        }
        case FSM_STATE_EXT_SET_NAME:
        {
            set_name(ch, ctx);
            break;
        }
        case FSM_STATE_EXT_SET_VALUE:
        {
            set_value(ch, ctx);
            break;
        }
        case FSM_STATE_INT_CHECK_COMMAND:
        {
            change_state_by_command(ctx);
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
            ctx->res = FSM_RES_EXIT;
            break;
        }
    }
}

void fsm(char ch, fsm_context_t *ctx)
{
    do
    {
        fsm_iter(ch, ctx);
    } while (ctx->res == FSM_RES_DO_INTERNAL);
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
