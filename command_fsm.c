#include "command_fsm.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <errno.h>

static union_storage_t *storage;
static us_elem_t temp_elem;

#define NEXT_STATE(result, new_state) \
do                                    \
{                                     \
    res = (result);                   \
    ctx->state = (new_state);         \
} while (0)

fsm_res_e fsm_init(fsm_context_t *ctx, union_storage_t *st)
{
    storage = st;
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
    else if (strcmp(ctx->input, "sum") == 0 || strcmp(ctx->input, "mul") == 0)
    {
        NEXT_STATE(FSM_RES_DO_INTERNAL, FSM_STATE_INT_SAVE_OPER);
        return res;
    }
    else
    {
        NEXT_STATE(FSM_RES_DO_INTERNAL, FSM_STATE_INT_ERROR);
        ctx->error = FSM_ERROR_WRONG_COMMAND;
    }

    clear_buffer(ctx);
    return res;
}

static fsm_res_e handl_input_char(char ch, fsm_context_t *ctx)
{
    fsm_res_e res = FSM_RES_CONTINUE;

    if (ctx->idx < INPUT_LENGTH - 1)
    {
        ctx->input[ctx->idx++] = ch;
    }
    else
    {
        NEXT_STATE(FSM_RES_DO_INTERNAL, FSM_STATE_INT_ERROR);
        ctx->error = FSM_ERROR_TOO_LONG_INPUT;
    }

    return res;
}

static fsm_res_e read_command(char ch, fsm_context_t *ctx)
{
    fsm_res_e res = FSM_RES_CONTINUE;
    if (isspace(ch) == 0)
    {
        res = handl_input_char(ch, ctx);
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
        res = handl_input_char(ch, ctx);
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
        res = handl_input_char(ch, ctx);
    }
    else
    {
        printf("Read set type: %s\n", ctx->input);
        if (strcmp(ctx->input, "int") == 0)
        {
            temp_elem.value.type = US_TYPE_INT;
        }
        else if (strcmp(ctx->input, "double") == 0)
        {
            temp_elem.value.type = US_TYPE_DOUBLE;
        }
        else
        {
            NEXT_STATE(FSM_RES_DO_INTERNAL, FSM_STATE_INT_ERROR);
            ctx->error = FSM_ERROR_WRONG_SET_TYPE;
            return res;
        }

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
        res = handl_input_char(ch, ctx);
    }
    else
    {
        printf("Read set name: %s\n", ctx->input);
        //save set name
        if (strlen(ctx->input) > NAME_LENGTH - 1)
        {
            NEXT_STATE(FSM_RES_DO_INTERNAL, FSM_STATE_INT_ERROR);
            ctx->error = FSM_ERROR_TOO_LONG_SET_NAME;
            return res;
        }

        strncpy(temp_elem.name, ctx->input, NAME_LENGTH);
        NEXT_STATE(FSM_RES_CONTINUE, FSM_STATE_EXT_SET_VALUE);
        clear_buffer(ctx);
    }

    return res;
}

static bool is_input_int(const char *input)
{
    for (int i = 0; i < strlen(input); i++)
    {
        if (isdigit(input[i]) == 0)
        {
            return false;
        }
    }

    return true;
}

static bool is_input_number(const char *input, us_type_e type)
{
    switch(type)
    {
        case US_TYPE_INT:
        {
            if (!is_input_int(input))
            {
                return false;
            }
            break;
        }
        case US_TYPE_DOUBLE:
        {
            char *end;
            double temp_db = strtod(input, &end);
            if (errno != 0 || *end != '\0')
            {
                return false;
            }
            break;
        }
        case US_TYPE_CLEAR:
        {
            break;
        }
    }

    return true;
}

static void save_value_in_temp(const char *input, us_elem_t *elem)
{
    switch(elem->value.type)
    {
        case US_TYPE_INT:
        {
            elem->value.un_data.int_m = atoi(input);
            printf("Read set int value: %d\n", elem->value.un_data.int_m);
            break;
        }
        case US_TYPE_DOUBLE:
        {
            elem->value.un_data.db_m = atof(input);
            printf("Read set double value: %f\n", elem->value.un_data.db_m);
            break;
        }
        case US_TYPE_CLEAR:
        {
            break;
        }
    }
}

static fsm_res_e set_value(char ch, fsm_context_t *ctx)
{
    fsm_res_e res = FSM_RES_CONTINUE;
    if (ch != '\n')
    {
        res = handl_input_char(ch, ctx);
    }
    else
    {
        if (!is_input_number(ctx->input, temp_elem.value.type))
        {
            NEXT_STATE(FSM_RES_DO_INTERNAL, FSM_STATE_INT_ERROR);
            ctx->error = FSM_ERROR_NOT_NUMBER;
            return res;
        }

        save_value_in_temp(ctx->input, &temp_elem);

        if (us_add(storage, temp_elem) == US_STORAGE_IS_FULL)
        {
            NEXT_STATE(FSM_RES_DO_INTERNAL, FSM_STATE_INT_ERROR);
            ctx->error = FSM_ERROR_FULL_STORAGE;
            return res;
        }

        us_clear_element(&temp_elem);
        NEXT_STATE(FSM_RES_CONTINUE, FSM_STATE_EXT_READ_COMMAND);
        clear_buffer(ctx);
    }

    return res;
}

static fsm_res_e input_skip(char ch, fsm_context_t *ctx)
{
    fsm_res_e res = FSM_RES_CONTINUE;
    if (ch == '\n')
    {
        NEXT_STATE(FSM_RES_CONTINUE, FSM_STATE_EXT_READ_COMMAND);
    }

    return res;
}

static bool is_sum = true;
#define ARGUMENTS_COUNT 2
static us_elem_t *args[ARGUMENTS_COUNT];
static fsm_res_e save_operation(fsm_context_t *ctx)
{
    fsm_res_e res;
    if (strcmp(ctx->input, "sum") == 0)
    {
        is_sum = true;
    }
    else if (strcmp(ctx->input, "mul") == 0)
    {
        is_sum = false;
    }

    NEXT_STATE(FSM_RES_CONTINUE, FSM_STATE_EXT_READ_ARG);
    clear_buffer(ctx);
    return res;
}

static fsm_res_e read_argument(char ch, fsm_context_t *ctx)
{
    static uint8_t arg_count = 0;
    fsm_res_e res = FSM_RES_CONTINUE;
    if (isspace(ch) == 0)
    {
        res = handl_input_char(ch, ctx);
    }
    else
    {
        printf("Read argument %s\n", ctx->input);
        if (strlen(ctx->input) >= NAME_LENGTH - 1)
        {
            if (ch == '\n')
            {
                strncat(ctx->input, "\n", 1);
            }
            ctx->error = FSM_ERROR_TOO_LONG_SET_NAME;
            NEXT_STATE(FSM_RES_DO_INTERNAL, FSM_STATE_INT_ERROR);
            return res;
        }

        args[arg_count] = us_get_by_name(storage, ctx->input);

        if (args[arg_count] == NULL)
        {
            if (ch == '\n')
            {
                strncat(ctx->input, "\n", 1);
            }
            NEXT_STATE(FSM_RES_DO_INTERNAL, FSM_STATE_INT_ERROR);
            ctx->error = FSM_ERROR_WRONG_CALC_ARG;
            return res;
        }

        arg_count++;

        if (arg_count == ARGUMENTS_COUNT)
        {
            if (is_sum)
            {
                if (args[0]->value.type == US_TYPE_INT && args[1]->value.type == US_TYPE_INT)
                {
                    printf("Result of sum: %d\n", args[0]->value.un_data.int_m + args[1]->value.un_data.int_m);
                }
                else if (args[0]->value.type == US_TYPE_DOUBLE && args[1]->value.type == US_TYPE_INT)
                {
                    printf("Result of sum: %f\n", args[0]->value.un_data.db_m + args[1]->value.un_data.int_m);
                }
                else if (args[0]->value.type == US_TYPE_INT && args[1]->value.type == US_TYPE_DOUBLE)
                {
                    printf("Result of sum: %f\n", args[0]->value.un_data.int_m + args[1]->value.un_data.db_m);
                }
                else
                {
                    printf("Result of sum: %f\n", args[0]->value.un_data.db_m + args[1]->value.un_data.db_m);
                }
            }
            else
            {
                if (args[0]->value.type == US_TYPE_INT && args[1]->value.type == US_TYPE_INT)
                {
                    printf("Result of sum: %d\n", args[0]->value.un_data.int_m * args[1]->value.un_data.int_m);
                }
                else if (args[0]->value.type == US_TYPE_DOUBLE && args[1]->value.type == US_TYPE_INT)
                {
                    printf("Result of sum: %f\n", args[0]->value.un_data.db_m * args[1]->value.un_data.int_m);
                }
                else if (args[0]->value.type == US_TYPE_INT && args[1]->value.type == US_TYPE_DOUBLE)
                {
                    printf("Result of sum: %f\n", args[0]->value.un_data.int_m * args[1]->value.un_data.db_m);
                }
                else
                {
                    printf("Result of sum: %f\n", args[0]->value.un_data.db_m * args[1]->value.un_data.db_m);
                }
            }
            arg_count = 0;
            NEXT_STATE(FSM_RES_CONTINUE, FSM_STATE_EXT_READ_COMMAND);
        }
        else
        {
            NEXT_STATE(FSM_RES_CONTINUE, FSM_STATE_EXT_READ_ARG);
        }
        clear_buffer(ctx);
    }

    return res;
}

static char fsm_error_messages[FSM_ERROR_COUNT][FSM_ERROR_MESSAGE_LENGTH] =
{
    [FSM_ERROR_WRONG_COMMAND] = "Wrong command",
    [FSM_ERROR_TOO_LONG_INPUT] = "Too long input",
    [FSM_ERROR_WRONG_SET_TYPE] = "Incorrect type",
    [FSM_ERROR_TOO_LONG_SET_NAME] = "Too long variable name",
    [FSM_ERROR_NOT_NUMBER] = "Set input is not a number",
    [FSM_ERROR_FULL_STORAGE] = "Cant add element: storage is full",
    [FSM_ERROR_WRONG_CALC_ARG] = "Argument not found in storage",
};

static fsm_res_e error_handler(fsm_context_t *ctx)
{
    fsm_res_e res;
    switch(ctx->error)
    {
        case FSM_ERROR_WRONG_COMMAND:
        case FSM_ERROR_TOO_LONG_INPUT:
        case FSM_ERROR_WRONG_SET_TYPE:
        case FSM_ERROR_TOO_LONG_SET_NAME:
        case FSM_ERROR_FULL_STORAGE:
        case FSM_ERROR_WRONG_CALC_ARG:
        {
            if (ctx->input[strlen(ctx->input) - 1] == '\n')
            {
                NEXT_STATE(FSM_RES_CONTINUE, FSM_STATE_EXT_READ_COMMAND);
            }
            else
            {
                NEXT_STATE(FSM_RES_CONTINUE, FSM_STATE_EXT_SKIP);
            }
            break;
        }
        case FSM_ERROR_NOT_NUMBER:
        {
            NEXT_STATE(FSM_RES_CONTINUE, FSM_STATE_EXT_READ_COMMAND);
            break;
        }
        case FSM_ERROR_WRONG_LOAD_FILENAME:
        {
            break;
        }
        case FSM_ERROR_COUNT:
        {
            break;
        }
    }
    printf("Error handler: %s\n", fsm_error_messages[ctx->error]);
    clear_buffer(ctx);
    us_clear_element(&temp_elem);

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
        case FSM_STATE_EXT_SKIP:
        {
            res = input_skip(ch, ctx);
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
        case FSM_STATE_EXT_READ_ARG:
        {
            res = read_argument(ch, ctx);
            break;
        }
        case FSM_STATE_INT_CHECK_COMMAND:
        {
            res = change_state_by_command(ctx);
            break;
        }
        case FSM_STATE_INT_SAVE_OPER:
        {
            res = save_operation(ctx);
            break;
        }
        case FSM_STATE_INT_ERROR:
        {
            res = error_handler(ctx);
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
