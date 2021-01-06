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

enum
{
    JSON_TYPE_NOVALUE,
    JSON_TYPE_STRING,
    JSON_TYPE_OBJECT,
    JSON_TYPE_ARRAY
};

struct json_value
{
    int type, value_count;
    char *key, *string_value;
    struct json_value **values;
};

static struct json_value *json_parse(char *json);
static void json_generate(char **json, int index, struct json_value *value);
static char *json_build(struct json_value *value);
static void json_delete(struct json_value *value);

static struct json_value *json_parse(char *json)
{
    char *trim = " \n";

    char *buffer = (char *)malloc(sizeof(char));
    buffer[0] = '\0';

    int i, j = 0;
    for(i = 0; json[i] != '\0'; i++)
    {
        if(json[i] == '\'' || json[i] == '\"')
        {
            if(j == 0)
                j = 1;
            else
                j = 0;
        }

        int skip = 0;
        if(j == 0)
        {
            int k;
            for(k = 0; trim[k] != '\0'; k++)
            {
                if(json[i] == trim[k])
                {
                    skip = 1;
                    break;
                }
            }
        }

        if(!skip)
        {
            int length = strlen(buffer);
            buffer = (char *)realloc(buffer, length + 2);

            buffer[length] = json[i];
            buffer[length + 1] = '\0';
        }
    }

    int length = strlen(buffer);

    struct json_value *container = (struct json_value *)malloc(sizeof(struct json_value));

    container->type = JSON_TYPE_OBJECT;
    container->value_count = 0;
    container->key = NULL;
    container->string_value = NULL;
    container->values = NULL;

    int count = 1;
    struct json_value **values = (struct json_value **)malloc(sizeof(struct json_value *));

    values[count - 1] = container;

    char *storage = (char *)malloc(sizeof(char));
    storage[0] = '\0';

    j = 0;
    for(i = 0; i < length; i++)
    {
        char c = buffer[i];

        switch(c)
        {
            case '{':
            case '[':
            {
                struct json_value *value = (struct json_value *)malloc(sizeof(struct json_value));

                if(c == '{')
                    value->type = JSON_TYPE_OBJECT;
                else if(c == '[')
                    value->type = JSON_TYPE_ARRAY;

                if(strlen(storage) > 0)
                {
                    value->key = strdup(storage);

                    storage = (char *)realloc(storage, sizeof(char));
                    storage[0] = '\0';
                }
                else
                    value->key = NULL;

                value->string_value = NULL;
                value->value_count = 0;
                value->values = NULL;

                if(values[count - 1]->values == NULL)
                    values[count - 1]->values = (struct json_value **)malloc(sizeof(struct json_value *));
                else
                    values[count - 1]->values = (struct json_value **)realloc(values[count - 1]->values, sizeof(struct json_value *) * (values[count - 1]->value_count + 1));

                values[count - 1]->value_count++;
                values[count - 1]->values[values[count - 1]->value_count - 1] = value;

                values = (struct json_value **)realloc(values, sizeof(struct json_value *) * (count + 1));

                count++;
                values[count - 1] = value;

                break;
            }
            case ']':
            case '}':
            {
                values = (struct json_value **)realloc(values, sizeof(struct json_value *) * (count - 1));
                count--;

                break;
            }
            case '\"':
            case '\'':
            {
                if(j == 0)
                {
                    j = 1;
                }
                else if(j == 1)
                {
                    if(i + 1 < length)
                    {
                        if(buffer[i + 1] != ':')
                        {
                            j = 0;

                            struct json_value *value = (struct json_value *)malloc(sizeof(struct json_value));

                            value->type = JSON_TYPE_NOVALUE;

                            if(strlen(storage) > 0)
                            {
                                value->key = strdup(storage);

                                storage = (char *)realloc(storage, sizeof(char));
                                storage[0] = '\0';
                            }
                            else
                                value->key = NULL;

                            value->string_value = NULL;
                            value->value_count = 0;
                            value->values = NULL;

                            if(values[count - 1]->values == NULL)
                                values[count - 1]->values = (struct json_value **)malloc(sizeof(struct json_value *));
                            else
                                values[count - 1]->values = (struct json_value **)realloc(values[count - 1]->values, sizeof(struct json_value *) * (values[count - 1]->value_count + 1));

                            values[count - 1]->value_count++;
                            values[count - 1]->values[values[count - 1]->value_count - 1] = value;
                        }
                        else
                        {
                            if(i + 2 < length)
                            {
                                if(buffer[i + 2] != '{' && buffer[i + 2] != '[')
                                {
                                    struct json_value *value = (struct json_value *)malloc(sizeof(struct json_value));

                                    value->type = JSON_TYPE_STRING;

                                    if(strlen(storage) > 0)
                                    {
                                        value->key = strdup(storage);

                                        storage = (char *)realloc(storage, sizeof(char));
                                        storage[0] = '\0';
                                    }
                                    else
                                        value->key = NULL;

                                    value->string_value = NULL;
                                    value->value_count = 0;
                                    value->values = NULL;

                                    if(values[count - 1]->values == NULL)
                                        values[count - 1]->values = (struct json_value **)malloc(sizeof(struct json_value *));
                                    else
                                        values[count - 1]->values = (struct json_value **)realloc(values[count - 1]->values, sizeof(struct json_value *) * (values[count - 1]->value_count + 1));

                                    values[count - 1]->value_count++;
                                    values[count - 1]->values[values[count - 1]->value_count - 1] = value;

                                    values = (struct json_value **)realloc(values, sizeof(struct json_value *) * (count + 1));

                                    count++;
                                    values[count - 1] = value;

                                    j = 3;
                                    i++;
                                }
                                else
                                {
                                    j = 0;
                                    i++;
                                }
                            }
                        }
                    }
                }
                else if(j == 3)
                    j = 4;
                else if(j == 4)
                {
                    if(strlen(storage) > 0)
                    {
                        values[count - 1]->string_value = strdup(storage);

                        storage = (char *)realloc(storage, sizeof(char));
                        storage[0] = '\0';
                    }
                    else
                        values[count - 1]->string_value = NULL;

                    values = (struct json_value **)realloc(values, sizeof(struct json_value *) * (count - 1));
                    count--;

                    j = 0;
                }
                break;
            }
            default:
            {
                if(j == 1 || j == 4)
                {
                    int storage_length = strlen(storage) + 1;
                    storage = (char *)realloc(storage, sizeof(char) * (storage_length + 1));

                    storage[storage_length - 1] = c;
                    storage[storage_length] = '\0';
                }
                break;
            }
        }
    }

    free(buffer);
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
