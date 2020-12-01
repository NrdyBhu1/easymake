#ifndef EASYMAKE_H
#define EASYMAKE_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct
{
    const char *project, *compiler, *output;
    const char **sources, **includes, **libraries, **compiler_options;
    int sources_count, includes_count, libraries_count, compiler_options_count;
} BuildOptions;

char *easymake_read_file(char *file);
BuildOptions easymake_build_options(char *buf);
void easymake_build_project(BuildOptions *build_options);

//concat util
static char *easymake_concat(const char *a, const char *b)
{
    char *result = (char *)malloc(strlen(a) + strlen(b) + 1);
    
    strcpy(result, a);
    strcat(result, b);
    
    return result;
}


static void easymake_failure_response(const char* error_message)
{
    printf("easymake: fatal error! %s\n",error_message);
    exit(1); 
}

#ifndef EASYMAKE_ASSERT
#define EASYMAKE_ASSERT(x , message) x == NULL ? easymake_failure_response(message) : 0
#endif


#endif
