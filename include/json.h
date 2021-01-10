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

/**
 * Json value types as enum.
 */
enum json_value_type
{
    JSON_TYPE_NOVALUE, /* 0 */
    JSON_TYPE_STRING,  /* 1 */
    JSON_TYPE_OBJECT,  /* 2 */
    JSON_TYPE_ARRAY    /* 3 */
};

/**
 * Represents a json value, which can be any
 * json type and have any type of value(s).
 *
 * In JSON (for string):
 *
 * "some_json_key":"some_json_value"
 *      ^^^              ^^^
 *      key             value
 *
 * In JSON (for object/array):
 *
 * "some_json_key": {
 *      ^^^
 *      key
 *
 *   "some_json_key": "value",      <|\
 *   "another_json_key: "value",    <| | Values
 *   "some_other_json_key: "value"  <|/
 * }
 *
 * type - holds the value type (from the enum above)
 * value_count - if the value is of type object or array, this holds
 *               the amount of child/subvalues the object or array has
 * key - holds the string for the key of the json_value
 * string_value - if the value of the json_value is a string, the string
 *                value will be here
 * values - the list of children/sub values the json value has if the
 *          json value is a object or array
 */
struct json_value
{
    int type, value_count;
    char *key, *string_value;
    struct json_value **values;
};

/**
 * Takes in a json string, and converts it into
 * a json tree.
 *
 * json - raw json string
 * returns - a struct json_value of type object
 *           with 'values' containing all json
 *           base objects
 */
static struct json_value *json_parse(char *json);

/**
 * A self-branching function to generate a json
 * string from a json tree.
 */
static void json_generate(char **json, int index, struct json_value *value);

/**
 * A function that simplifies the use of json_generate.
 */
static char *json_build(struct json_value *value);

/**
 * A function to cleanup and free all memory used by
 * a json tree.
 */
static void json_delete(struct json_value *value);

/*----------------------------------------------------------------------------*/
/*                          Function Implementations                          */
/*----------------------------------------------------------------------------*/

enum parser_state
{
    IDLE,            /* 0 */
    PARSING_KEY,     /* 1 */
    CHECKING_TYPE,   /* 2 */
    PARSING_STRING,  /* 3 */
};

static struct json_value *json_parse(char *json)
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
    for(i = 0; json[i] != 0; i++)
    {
        char c = json[i];

        switch(state)
        {
            case IDLE:
            {
                switch(c)
                {
                    case '\'':
                    case '\"':
                    {
                        printf("Switching state to: PARSING_KEY\n");

                        state = PARSING_KEY;
                        break;
                    }
                    case '{':
                    case '[':
                    {
                        printf("Switching state to: CHECKING_TYPE\n");

                        i--;
                        state = CHECKING_TYPE;

                        break;
                    }
                    case '}':
                    case ']':
                    {
                        printf("Stepping backwards...\n");

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
                        printf("KEY: %c\n", c);

                        if(c == '\\')
                        {
                            printf("Found escape code: %c\n", json[i + 1]);
                            if(json[i + 1] == 'n')
                            {
                                c = '\n';
                                i++;
                            }
                            else if(json[i + 1] == '\'' || json[i + 1] == '\"')
                            {
                                c = json[i + 1];
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
                        else
                            value->key = NULL;

                        printf("STRING: %s\n", value->key);

                        value->string_value = NULL;
                        value->value_count = 0;
                        value->values = NULL;

                        if(values[value_count - 1]->values == NULL)
                            values[value_count - 1]->values = (struct json_value **)malloc(sizeof(struct json_value *));
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
                        else
                            value->key = NULL;

                        printf("OBJECT/ARRAY (%d): %s\n", value->type, value->key);

                        value->string_value = NULL;
                        value->value_count = 0;
                        value->values = NULL;

                        if(values[value_count - 1]->values == NULL)
                            values[value_count - 1]->values = (struct json_value **)malloc(sizeof(struct json_value *));
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
                        else
                            value->key = NULL;

                        printf("NO_VALUE: %s\n", value->key);

                        value->string_value = NULL;
                        value->value_count = 0;
                        value->values = NULL;

                        if(values[value_count - 1]->values == NULL)
                            values[value_count - 1]->values = (struct json_value **)malloc(sizeof(struct json_value *));
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
                            values[value_count - 1]->string_value = strdup(storage);

                            storage = (char *)realloc(storage, sizeof(char));
                            storage_count = 1;

                            storage[0] = 0;
                        }

                        printf("STRING_VALUE: %s\n", values[value_count-1]->string_value);

                        state = IDLE;

                        break;
                    }
                    default:
                    {
                        printf("STORING %c\n", c);

                        if(c == '\\')
                        {
                            if(json[i + 1] == 'n')
                            {
                                c = '\n';
                                i++;
                            }
                            else if(json[i + 1] == '\'' || json[i + 1] == '\"')
                            {
                                c = json[i + 1];
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

        printf("STORAGE: %s\n  -  VALUE_COUNT: %d\n", storage, value_count);
    }

    free(storage);

    values = realloc(values, 1);
    free(values);

    return container;
}

static void json_generate(char **json, int index, struct json_value *value)
{
    if(value->type == JSON_TYPE_OBJECT || value->type == JSON_TYPE_ARRAY)
    {
        int i;
        for(i = 0; i < index; i++)
            *json = strcat(*json, "    ");

        if(value->key != NULL)
        {
            *json = strcat(*json, "\"");
            *json = strcat(*json, value->key);
            *json = strcat(*json, "\": ");
        }

        if(value->type == JSON_TYPE_OBJECT)
            *json = strcat(*json, "{\n");
        else if(value->type == JSON_TYPE_ARRAY)
            *json = strcat(*json, "[\n");

        for(i = 0; i < value->value_count; i++)
        {
            struct json_value *arrval = value->values[i];

            json_generate(json, index + 1, arrval);

            if(i + 1 < value->value_count)
                *json = strcat(*json, ",\n");
            else
                *json = strcat(*json, "\n");
        }

        for(i = 0; i < index; i++)
            *json = strcat(*json, "    ");

        if(value->type == JSON_TYPE_OBJECT)
            *json = strcat(*json, "}");
        else if(value->type == JSON_TYPE_ARRAY)
            *json = strcat(*json, "]");
    }
    else
    {
        if(value->type == JSON_TYPE_STRING)
        {
            int i;
            for(i = 0; i < index; i++)
                *json = strcat(*json, "    ");

            if(value->key != NULL)
            {
                *json = strcat(*json, "\"");
                *json = strcat(*json, value->key);
                *json = strcat(*json, "\": \"");
            }

            if(value->string_value != NULL)
                *json = strcat(*json, value->string_value);

            *json = strcat(*json, "\"");
        }
        else if(value->type == JSON_TYPE_NOVALUE)
        {
            int i;
            for(i = 0; i < index; i++)
                *json = strcat(*json, "    ");

            if(value->key != NULL)
            {
                *json = strcat(*json, "\"");
                *json = strcat(*json, value->key);
                *json = strcat(*json, "\"");
            }
        }
    }
}

static char *json_build(struct json_value *value)
{
    char *json = (char *)malloc(sizeof(char));
    json[0] = '\0';

    json_generate(&json, 0, value);

    return json;
}

static void json_delete(struct json_value *value)
{
    if(value->values != NULL)
    {
        int i;
        for(i = 0; i < value->value_count; i++)
        {
            struct json_value *arrval = value->values[i];

            if(arrval != NULL)
                json_delete(arrval);
        }

        free(value->values);
    }

    if(value->key != NULL)
        free(value->key);

    if(value->string_value != NULL)
        free(value->string_value);

    free(value);
}

#endif
