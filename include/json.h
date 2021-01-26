/* json.h - a simple, lightweight, easy to use json parser
 *
 * Copyright (c) 2021 Cleanware
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
*/

#ifndef JSON_H
#define JSON_H

#include <stdlib.h>
#include <string.h>

/*---------------------------------------------------------------------------*/
/*                              Data Structures                              */
/*---------------------------------------------------------------------------*/

enum json_value_type
{
    JSON_TYPE_NOVALUE, /* 0 */
    JSON_TYPE_STRING,  /* 1 */
    JSON_TYPE_OBJECT,  /* 2 */
    JSON_TYPE_ARRAY    /* 3 */
};

struct json_value
{
    int type, value_count;
    char *key, *string_value;
    struct json_value **values;
};

/*---------------------------------------------------------------------------------*/
/*                              Function Declarations                              */
/*---------------------------------------------------------------------------------*/

/**
 * Takes in a json string and returns
 * a json tree.
 */
static struct json_value *json_parse(char *buffer);

/**
 * Cleans up and frees all memory used by
 * a json tree.
 */
static void json_delete(struct json_value *value);

/*------------------------------------------------------------------------------------*/
/*                              Function Implementations                              */
/*------------------------------------------------------------------------------------*/

enum parser_state
{
    IDLE,            /* 0 */
    PARSING_KEY,     /* 1 */
    CHECKING_TYPE,   /* 2 */
    PARSING_STRING,  /* 3 */
};

static struct json_value *json_parse(char *buffer)
{
    struct json_value *container = (struct json_value *)malloc(sizeof(struct json_value));

    container->type = JSON_TYPE_OBJECT;
    container->value_count = 0;
    container->key = NULL;
    container->string_value = NULL;
    container->values = NULL;

    int value_count = 1;
    struct json_value **values = (struct json_value **)malloc(sizeof(struct json_value *));

    values[value_count - 1] = container;

    int storage_count = 1;
    char *storage = (char *)malloc(sizeof(char));

    storage[0] = 0;

    /* parser state */
    int state = IDLE;

    int i;
    for(i = 0; buffer[i] != 0; i++)
    {
        char c = buffer[i];

        switch(state)
        {
            case IDLE:
            {
                switch(c)
                {
                    case '\'':
                    case '\"':
                    {
                        state = PARSING_KEY;
                        break;
                    }
                    case '{':
                    case '[':
                    {
                        i--;
                        state = CHECKING_TYPE;

                        break;
                    }
                    case '}':
                    case ']':
                    {
                        if(value_count > 1)
                        {
                            values = (struct json_value **)realloc(values, sizeof(struct json_value *) * (value_count - 1));
                            value_count--;
                        }

                        break;
                    }
                }

                break;
            }
            case PARSING_KEY:
            {
                switch(c)
                {
                    case '\'':
                    case '\"':
                    {
                        state = CHECKING_TYPE;
                        break;
                    }
                    default:
                    {
                        if(c == '\\')
                        {
                            if(buffer[i + 1] == 'n')
                            {
                                c = '\n';
                                i++;
                            }
                            else if(buffer[i + 1] == '\'' || buffer[i + 1] == '\"')
                            {
                                c = buffer[i + 1];
                                i++;
                            }
                        }

                        storage = (char *)realloc(storage, sizeof(char) * (storage_count + 1));

                        storage[storage_count - 1] = c;
                        storage[storage_count] = 0;

                        storage_count++;

                        break;
                    }
                }

                break;
            }
            case CHECKING_TYPE:
            {
                switch(c)
                {
                    case '\'':
                    case '\"':
                    {
                        /* string value */

                        struct json_value *value = (struct json_value *)malloc(sizeof(struct json_value));

                        value->type = JSON_TYPE_STRING;

                        if(storage_count > 1)
                        {
                            value->key = strdup(storage);

                            storage = (char *)realloc(storage, sizeof(char));
                            storage_count = 1;

                            storage[0] = 0;
                        }
                        else value->key = NULL;

                        value->string_value = NULL;
                        value->value_count = 0;
                        value->values = NULL;

                        if(values[value_count - 1]->values == NULL) values[value_count - 1]->values = (struct json_value **)malloc(sizeof(struct json_value *));
                        else
                            values[value_count - 1]->values = (struct json_value **)realloc(values[value_count - 1]->values, sizeof(struct json_value *) * (values[value_count - 1]->value_count + 1));

                        values[value_count - 1]->value_count++;
                        values[value_count - 1]->values[values[value_count - 1]->value_count - 1] = value;

                        state = PARSING_STRING;

                        break;
                    }
                    case '{':
                    case '[':
                    {
                        /* object/array value */

                        struct json_value *value = (struct json_value *)malloc(sizeof(struct json_value));

                        if(c == '{') value->type = JSON_TYPE_OBJECT;
                        else if(c == '[') value->type = JSON_TYPE_ARRAY;

                        if(storage_count > 1)
                        {
                            value->key = strdup(storage);

                            storage = (char *)realloc(storage, sizeof(char));
                            storage_count = 1;

                            storage[0] = 0;
                        }
                        else value->key = NULL;

                        value->string_value = NULL;
                        value->value_count = 0;
                        value->values = NULL;

                        if(values[value_count - 1]->values == NULL) values[value_count - 1]->values = (struct json_value **)malloc(sizeof(struct json_value *));
                        else
                            values[value_count - 1]->values = (struct json_value **)realloc(values[value_count - 1]->values, sizeof(struct json_value *) * (values[value_count - 1]->value_count + 1));

                        values[value_count - 1]->value_count++;
                        values[value_count - 1]->values[values[value_count - 1]->value_count - 1] = value;

                        values = (struct json_value **)realloc(values, sizeof(struct json_value *) * (value_count + 1));
                        value_count++;

                        values[value_count - 1] = value;

                        state = IDLE;

                        break;
                    }
                    case ',':
                    case '}':
                    case ']':
                    {
                        /* no value */

                        struct json_value *value = (struct json_value *)malloc(sizeof(struct json_value));

                        value->type = JSON_TYPE_NOVALUE;

                        if(storage_count > 1)
                        {
                            value->key = strdup(storage);

                            storage = (char *)realloc(storage, sizeof(char));
                            storage_count = 1;

                            storage[0] = 0;
                        }
                        else value->key = NULL;

                        value->string_value = NULL;
                        value->value_count = 0;
                        value->values = NULL;

                        if(values[value_count - 1]->values == NULL) values[value_count - 1]->values = (struct json_value **)malloc(sizeof(struct json_value *));
                        else
                            values[value_count - 1]->values = (struct json_value **)realloc(values[value_count - 1]->values, sizeof(struct json_value *) * (values[value_count - 1]->value_count + 1));

                        values[value_count - 1]->value_count++;
                        values[value_count - 1]->values[values[value_count - 1]->value_count - 1] = value;

                        state = IDLE;
                        i--;

                        break;
                    }
                }

                break;
            }
            case PARSING_STRING:
            {
                switch(c)
                {
                    case '\'':
                    case '\"':
                    {
                        if(storage_count > 1)
                        {
                            values[value_count - 1]->values[values[value_count - 1]->value_count - 1]->string_value = strdup(storage);

                            storage = (char *)realloc(storage, sizeof(char));
                            storage_count = 1;

                            storage[0] = 0;
                        }

                        state = IDLE;

                        break;
                    }
                    default:
                    {
                        if(c == '\\')
                        {
                            if(buffer[i + 1] == 'n')
                            {
                                c = '\n';
                                i++;
                            }
                            else if(buffer[i + 1] == '\'' || buffer[i + 1] == '\"')
                            {
                                c = buffer[i + 1];
                                i++;
                            }
                        }

                        storage = (char *)realloc(storage, sizeof(char) * (storage_count + 1));

                        storage[storage_count - 1] = c;
                        storage[storage_count] = 0;

                        storage_count++;

                        break;
                    }
                }

                break;
            }
        }
    }

    free(storage);

    values = realloc(values, 1);
    free(values);

    return container;
}

static void json_delete(struct json_value *value)
{
    if(value->values != NULL)
    {
        int i;
        for(i = 0; i < value->value_count; i++)
        {
            struct json_value *arrval = value->values[i];

            if(arrval != NULL) json_delete(arrval);
        }

        free(value->values);
    }

    if(value->key != NULL) free(value->key);
    if(value->string_value != NULL) free(value->string_value);

    free(value);
}

#endif
