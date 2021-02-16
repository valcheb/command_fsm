#ifndef UNION_STORAGE_H_
#define UNION_STORAGE_H_

#include "stdint.h"

#define NAME_LENGTH 10

typedef enum
{
    US_RES_OK = 0,
    US_STORAGE_IS_FULL
} us_res_e;

typedef union
{
    int    int_m;
    double db_m;
} int_db_type_u;

typedef enum
{
    US_TYPE_CLEAR = 0,
    US_TYPE_INT,
    US_TYPE_DOUBLE
} us_type_e;

typedef struct
{
    us_type_e     type;
    int_db_type_u un_data;
} us_type_t;

typedef struct
{
    char      name[NAME_LENGTH];
    us_type_t value;
} us_elem_t;

typedef struct
{
    us_elem_t *data;
    int       current_size;
    int       size;
} union_storage_t;

us_res_e us_init(union_storage_t *storage, us_elem_t *data, int size);
us_res_e us_add(union_storage_t *storage, us_elem_t elem);
us_elem_t *us_get_by_name(union_storage_t *storage, char *name);
us_res_e us_clear_element(us_elem_t *elem);

#endif /*UNION_STORAGE_H_*/
