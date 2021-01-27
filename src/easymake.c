/* easymake - a simple, lightweight, easy to use build system
 *
 * Copyright (C) 2021 Cleanware
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "cutils/parser/json/json.h"
#include "cutils/thread/thread.h"
#include <stdio.h>
#include <stdlib.h>

#if defined(_WIN32)
    #define PLATFORM "windows"
#elif defined(__MACH__)
    #define PLATFORM "macos"
#elif defined(__APPLE__)
    #define PLATFORM "apple"
#elif defined(__ANDROID__)
    #define PLATFORM "android"
#elif defined(__linux__)
    #define PLATFORM "linux"
#elif defined(BSD)
    #define PLATFORM "bsd"
#elif defined(unix) || defined(__unix__) || defined(__unix)
    #define PLATFORM "unix"
#elif
    #define PLATFORM "unknown"
#endif

void *run_cmd(void *data);
char *easymake_read_file(char *file_path);
char *easymake_format_string(char *str, int variable_count, struct json_value **variables);
char **easymake_parse_commands(struct json_value *value, char **user_targets);
int easymake_build(char *file, char **targets, int verbose, int thread_count);

void *run_cmd(void *data)
{
    char *cmd = (char *)data;
    system(cmd);
}

char *easymake_read_file(char *file_path)
{
    FILE *file = fopen(file_path, "r");

    if(file == NULL) return NULL;

    int count = 1;
    char *contents = (char *)malloc(sizeof(char));

    char c = fgetc(file);

    while(c != EOF)
    {
        contents = (char *)realloc(contents, sizeof(char) * (count + 1));

        contents[count - 1] = c;
        contents[count] = '\0';

        count++;

        c = fgetc(file);
    }

    fclose(file);

    return contents;
}

char *easymake_format_string(char *str, int variable_count, struct json_value **variables)
{
    int instanced = 0;

    if(str[0] == '!') instanced = 1;

    int formatted_count = 1;
    char *formatted = (char *)malloc(sizeof(char));

    formatted[0] = 0;

    int storage_count = 1;
    char *storage = (char *)malloc(sizeof(char));

    storage[0] = 0;

    int j, k = 0, l;
    for(j = 0; str[j] != 0; j++)
    {
        char c = str[j];

        if(k == 0)
        {
            if(c == '#')
            {
                char c2 = str[j + 1];

                if(c2 != 0)
                {
                    if(c2 == '(')
                    {
                        j++;
                        k = 1;
                    }
                }

                continue;
            }

            formatted = (char *)realloc(formatted, sizeof(char) * (formatted_count + 1));

            formatted[formatted_count - 1] = c;
            formatted[formatted_count] = 0;

            formatted_count++;
        }
        else if(k == 1)
        {
            if(c != ')')
            {
                storage = (char *)realloc(storage, sizeof(char) * (storage_count + 1));

                storage[storage_count - 1] = c;
                storage[storage_count] = 0;

                storage_count++;
            }
            else
            {
                for(l = 0; storage[l] != 0; l++)
                {
                    if(storage[l] == '#') storage = easymake_format_string(storage, variable_count, variables);
                }

                for(l = 0; l < variable_count; l++)
                {
                    struct json_value *variable = variables[l];

                    if(strcmp(variable->key, storage) == 0)
                    {
                        if(variable->type == JSON_TYPE_STRING)
                        {
                            int m;
                            for(m = 0; variable->string_value[m] != 0; m++)
                            {
                                formatted = (char *)realloc(formatted, sizeof(char) * (formatted_count + 1));

                                formatted[formatted_count - 1] = variable->string_value[m];
                                formatted[formatted_count] = 0;

                                formatted_count++;
                            }
                        }
                        else if(variable->type == JSON_TYPE_ARRAY)
                        {
                            if(instanced)
                            {
                                /* not implemented and it causes build error on top of that */
                                /*int m;
                                for(m = 0; m < variable->value_count; m++)
                                {
                                    if(arr_value->type == JSON_TYPE_NOVALUE)
                                    {
                                        
                                    }
                                }*/
                            }
                            else
                            {
                                int m, n;
                                for(m = 0; m < variable->value_count; m++)
                                {
                                    struct json_value *arr_value = variable->values[m];

                                    if(arr_value->type == JSON_TYPE_NOVALUE)
                                    {
                                        for(n = 0; arr_value->key[n] != 0; n++)
                                        {
                                            formatted = (char *)realloc(formatted, sizeof(char) * (formatted_count + 1));

                                            formatted[formatted_count - 1] = arr_value->key[n];
                                            formatted[formatted_count] = 0;

                                            formatted_count++;
                                        }

                                        if(m + 1 < variable->value_count)
                                        {
                                            formatted = (char *)realloc(formatted, sizeof(char) * (formatted_count + 1));

                                            formatted[formatted_count - 1] = ' ';
                                            formatted[formatted_count] = 0;

                                            formatted_count++;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }

                storage = (char *)realloc(storage, sizeof(char));
                storage_count = 1;

                storage[storage_count - 1] = 0;

                k = 0;
            }
        }
    }

    free(storage);

    return formatted;
}

char **easymake_parse_commands(struct json_value *value, char **user_targets)
{
    int command_count = 0;
    char **commands = NULL;

    int target_count = 1;
    char **targets = (char **)malloc(sizeof(char *));

    targets[0] = strdup(PLATFORM);

    if(user_targets[0] != NULL)
    {
        int i;
        for(i = 0; user_targets[i] != NULL; i++)
        {
            targets = (char **)realloc(targets, sizeof(char *) * (target_count + 1));
            target_count++;

            targets[target_count - 1] = user_targets[i];
        }
    }

    int variable_count = 1;
    struct json_value **variables = (struct json_value **)malloc(sizeof(struct json_value *));

    variables[0] = (struct json_value *)malloc(sizeof(struct json_value));
    variables[0]->type = JSON_TYPE_STRING;
    variables[0]->value_count = 0;
    variables[0]->key = strdup("os");
    variables[0]->string_value = strdup(PLATFORM);
    variables[0]->values = NULL;

    int value_count = 1;
    struct json_value **values = (struct json_value **)malloc(sizeof(struct json_value *));

    values[0] = value;

    int index_count = 1;
    int *index = (int *)malloc(sizeof(int));

    for(index[index_count - 1] = 0; index[index_count - 1] < values[value_count - 1]->value_count; index[index_count - 1]++)
    {
        struct json_value *sub_value = values[value_count - 1]->values[index[index_count - 1]];

        switch(sub_value->type)
        {
            case JSON_TYPE_STRING:
            {
                if(strcmp(sub_value->key, "#build") == 0)
                {
                    targets = (char **)realloc(targets, sizeof(char *) * (target_count + 1));
                    target_count++;

                    targets[target_count - 1] = sub_value->string_value;

                    break;
                }

                if(strcmp(sub_value->key, "#default") == 0)
                {
                    if(target_count <= 1)
                    {
                        targets = (char **)realloc(targets, sizeof(char *) * (target_count + 1));
                        target_count++;

                        targets[target_count - 1] = sub_value->string_value;

                        break;
                    }
                }

                if(variable_count == 0)
                {
                    variables = (struct json_value **)realloc(variables, sizeof(struct json_value *) * (variable_count + 1));
                    variable_count++;

                    variables[variable_count - 1] = sub_value;
                }
                else
                {
                    int j;
                    for(j = 0; j < variable_count; j++)
                    {
                        if(strcmp(sub_value->key, variables[j]->key) == 0)
                        {
                            variables[j] = sub_value;
                            j = -1;

                            break;
                        }
                    }

                    if(j != -1)
                    {
                        variables = (struct json_value **)realloc(variables, sizeof(struct json_value *) * (variable_count + 1));
                        variable_count++;

                        variables[variable_count - 1] = sub_value;
                    }
                }

                char *formatted = easymake_format_string(sub_value->string_value, variable_count, variables);

                free(sub_value->string_value);

                sub_value->string_value = formatted;

                break;
            }
            case JSON_TYPE_OBJECT:
            {
                int j;
                for(j = 0; j < target_count; j++)
                {
                    if(strcmp(sub_value->key, targets[j]) == 0)
                    {
                        values = (struct json_value **)realloc(values, sizeof(struct json_value *) * (value_count + 1));
                        value_count++;

                        values[value_count - 1] = sub_value;

                        index = (int *)realloc(index, sizeof(int) * (index_count + 1));
                        index_count++;

                        index[index_count - 1] = -1;

                        break;
                    }
                }

                break;
            }
            case JSON_TYPE_ARRAY:
            {
                int j;
                for(j = 0; j < sub_value->value_count; j++)
                {
                    struct json_value *arr_value = sub_value->values[j];

                    if(arr_value->type == JSON_TYPE_NOVALUE)
                    {
                        char *formatted = easymake_format_string(arr_value->key, variable_count, variables);

                        free(arr_value->key);

                        arr_value->key = formatted;
                    }
                }

                if(strcmp(sub_value->key, "#build") == 0)
                {
                    for(j = 0; j < sub_value->value_count; j++)
                    {
                        struct json_value *arr_value = sub_value->values[j];

                        if(arr_value->type == JSON_TYPE_NOVALUE)
                        {
                            targets = (char **)realloc(targets, sizeof(char *) * (target_count + 1));
                            target_count++;

                            targets[target_count - 1] = arr_value->key;
                        }
                    }

                    break;
                }

                if(variable_count == 0)
                {
                    variables = (struct json_value **)realloc(variables, sizeof(struct json_value *) * (variable_count + 1));
                    variable_count++;

                    variables[variable_count - 1] = sub_value;
                }
                else
                {
                    int j;
                    for(j = 0; j < variable_count; j++)
                    {
                        if(strcmp(sub_value->key, variables[j]->key) == 0)
                        {
                            variables[j] = sub_value;
                            j = -1;

                            break;
                        }
                    }

                    if(j != -1)
                    {
                        variables = (struct json_value **)realloc(variables, sizeof(struct json_value *) * (variable_count + 1));
                        variable_count++;

                        variables[variable_count - 1] = sub_value;
                    }
                }

                break;
            }
        }

        if(value_count > 0 && index[index_count - 1] + 1 == values[value_count - 1]->value_count)
        {
            values = (struct json_value **)realloc(values, sizeof(struct json_value **) * (value_count - 1));
            value_count--;

            index = (int *)realloc(index, sizeof(int) * (index_count - 1));
            index_count--;
        }

        if(value_count == 0) break;
    }

    int i;
    for(i = 0; i < variable_count; i++)
    {
        int j;
        for(j = 0; j < target_count; j++)
        {
            if(strcmp(variables[i]->key, targets[j]) == 0)
            {
                if(variables[i]->type == JSON_TYPE_STRING)
                {
                    if(command_count == 0)
                    {
                        commands = (char **)malloc(sizeof(char *));
                        command_count++;

                        commands[command_count - 1] = strdup(variables[i]->string_value);
                    }
                    else
                    {
                        commands = (char **)realloc(commands, sizeof(char *) * (command_count + 1));
                        command_count++;

                        commands[command_count - 1] = strdup(variables[i]->string_value);
                    }
                }
                else if(variables[i]->type == JSON_TYPE_ARRAY)
                {
                    int k;
                    for(k = 0; k < variables[i]->value_count; k++)
                    {
                        if(command_count == 0)
                        {
                            commands = (char **)malloc(sizeof(char *));
                            command_count++;

                            commands[command_count - 1] = strdup(variables[i]->values[k]->key);
                        }
                        else
                        {
                            commands = (char **)realloc(commands, sizeof(char *) * (command_count + 1));
                            command_count++;

                            commands[command_count - 1] = strdup(variables[i]->values[k]->key);
                        }
                    }
                }
            }
        }
    }

    if(command_count == 0)
    {
        commands = (char **)malloc(sizeof(char *));
        command_count++;

        commands[command_count - 1] = NULL;
    }
    else
    {
        commands = (char **)realloc(commands, sizeof(char *) * (command_count + 1));
        command_count++;

        commands[command_count - 1] = NULL;
    }

    free(targets[0]);
    free(targets);

    free(variables[0]->string_value);
    free(variables[0]->key);
    free(variables[0]);
    free(variables);

    return commands;
}

int easymake_build(char *file, char **targets, int verbose, int thread_count)
{
    char *contents = easymake_read_file(file);

    if(contents == NULL)
    {
        printf("easymake: error: file \'%s\' not found\n", file);
        return -1;
    }

    struct json_value *tree = json_parse(contents);

    free(contents);

    if(tree->type != JSON_TYPE_OBJECT || tree->value_count == 0)
    {
        json_delete(tree);

        printf("easymake: error: invalid buildfile\n");
        return -2;
    }

    char **commands = easymake_parse_commands(tree->values[0], targets);

    json_delete(tree);

    if(commands[0] == NULL)
    {
        free(commands);

        printf("easymake: error: no target specified\n");

        return -3;
    }

    int count;
    for(count = 0; commands[count] != NULL; count++);

    int thread_index = 0;
    thread_t threads[thread_count];

    int i, j;
    for(i = 0; commands[i] != NULL; i++)
    {
        for(j = i; j < thread_count; j++)
        {
            char *cmd = commands[j];

            if(cmd[0] == '!')
            {
                threads[thread_index] = create_thread(run_cmd, (void *)(cmd + 1));
                thread_index++;
                i++;
            }
            else break;
        }

        if(thread_index > 0)
        {
            for(j = 0; j <= thread_index; j++) join_thread(threads[j]);
            thread_index = 0;
        }
        else system(commands[i]);
    }

    for(; i >= 0; i--) free(commands[i]);

    free(commands);

    return 0;
}

int main(int argc, char *argv[])
{
    int target_count = 0, verbose = 0, thread_count = 1;
    char *file = "build.ezmk", *targets[argc];

    if(argc > 1)
    {
        int i;
        for(i = 1; i < argc; i++)
        {
            char *arg = argv[i];

            if(strcmp(arg, "-v") == 0 || strcmp(arg, "--version") == 0)
            {
                printf("easymake 1.0.0\nCopyright (C) 2021 Cleanware\nThis is free software; see the source for copying conditions.  There is NO\nwarranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n");
                return 0;
            }
            else if(strcmp(arg, "-h") == 0 || strcmp(arg, "--help") == 0)
            {
                printf("Usage: easymake [options] targets...\nOptions:\n");

                printf(" -v / --version      -     Display easymake version information.\n");
                printf(" -h / --help         -     Display this help page.\n");
                printf(" -f / --file         -     Specify build file.\n");
                printf(" -d / --debug        -     Enable verbose output.\n");
                printf(" -j / --jobs         -     Specify thread count.\n");

                return 0;
            }
            else if(strcmp(arg, "-f") == 0 || strcmp(arg, "--file") == 0)
            {
                if(i + 1 < argc)
                {
                    file = argv[i + 1];
                    i++;
                }
            }
            else if(strcmp(arg, "-d") == 0 || strcmp(arg, "--debug") == 0)
            {
                verbose = 1;
            }

            else if(strcmp(arg, "-j") == 0 || strcmp(arg, "--jobs") == 0)
            {
                if(i + 1 < argc)
                {
                    thread_count = (int)atoi(argv[i + 1]);
                    i++;
                }
            }
            else
            {
                targets[target_count] = argv[i];
                target_count++;
            }
        }
    }

    targets[target_count] = NULL;

    return easymake_build(file, targets, verbose, thread_count);
}
