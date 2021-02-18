#include "union_storage.h"

#include <string.h>

us_res_e us_init(union_storage_t *storage, us_elem_t *data, int size)
{
    storage->data = data;
    storage->current_size = 0;
    storage->size = size;
    return US_RES_OK;
}

us_res_e us_add(union_storage_t *storage, us_elem_t elem)
{
    if (storage->current_size >= storage->size)
    {
        return US_STORAGE_IS_FULL;
    }

    storage->data[storage->current_size] = elem;
    storage->current_size++;

    return US_RES_OK;
}

us_elem_t *us_get_by_name(const union_storage_t *storage, const char *name)
{
    for (int idx = 0; idx < storage->size; idx++)
    {
        us_elem_t *de = &storage->data[idx];

        if (strncmp(de->name, name, NAME_LENGTH) == 0)
        {
            return de;
        }
    }

    return NULL;
}

us_res_e us_clear_element(us_elem_t *elem)
{
    memset(elem->name, '\0', NAME_LENGTH);
    memset(&elem->value, 0, sizeof(elem->value));
    return US_RES_OK;
}
