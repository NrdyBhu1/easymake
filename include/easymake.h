#ifndef EASYMAKE_H
#define EASYMAKE_H

#include "easylib.h"
#include <stdio.h>

typedef struct
{
  const char *project, *compiler, *output;
  const char **sources, **includes, **libraries, **compiler_options;
  int sources_count, includes_count, libraries_count, compiler_options_count;
} BuildOptions;

char *easymake_read_file(char *file);
BuildOptions easymake_build_options(char *buf, char *target);
void easymake_build_project(BuildOptions *boptions);

#endif
