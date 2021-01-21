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

#include "json.h"
#include "thread.h"
#include <stdio.h>
#include <stdlib.h>

#if defined(_WIN64)
    #define PLATFORM "win64"
#elif defined(_WIN32)
    #define PLATFORM "win32"
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

struct job
{
    int async, command_count;
    char **commands;
};

void *run_command(void *arg);
char *easymake_read_file(char *file_path);
char *easymake_format_string(char *str, int variable_count, struct json_value **variables);
int easymake_build(char *file, char **targets, int verbose);

void *run_command(void *arg)
{
    char *cmd = (char *)arg;
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

void easymake_format_string(char *str, int *variable_count, struct json_value ***variables)
{
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

void easymake_parse_jobs(struct json_value *tree, int *job_count, struct job ***jobs, int *variable_count, struct json_value ***variables, char **targets)
{
    int i;
    for(i = 0; i < tree->value_count; i++)
    {
        struct json_value *sub_value = tree->values[i];

        switch(sub_value->type)
        {
            case JSON_TYPE_STRING:
            {
                if(sub_value
            }
        }
    }
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

    int job_count = 0;
    struct job **jobs = (struct job **)malloc(sizeof(struct job *));

    easymake_parse_jobs(tree, &job_count, &jobs);

    int i, j, k;
    for(i = 0; i < job_count; i++)
    {
        struct job *job = jobs[i];

        k = 0;
        if(job->async)
        {
            thread_t threads[thread_count];

            while(k < job->command_count)
            {
                for(j = 0; j < thread_count; j++)
                {
                    if(k >= job->command_count) break;

                    threads[thread_count] = create_thread(run_command, (void *)job->commands[k]);
                    k++;
                }

                for(j = 0; j < thread_count; j++) join_thread(threads[j]);
            }
        }
        else
        {
            for(j = 0; j < job->command_count; j++)
            {
                system(job->commands[j]);
            }
        }
    }

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
                    thread_count = (int)strtol(argv[i + 1]);
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
