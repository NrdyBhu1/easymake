#ifndef EASYMAKE_H
#define EASYMAKE_H

#include <stdio.h>
#include <stdlib.h>

typedef struct
{
  char *description, compiler, output_file;
  char **sources, includes, libraries, compiler_options;
  int sources_count, includes_count, libraries_count, compiler_options_count;
} BuildOptions;

char *easymake_read_file(char *file);
BuildOptions easymake_build_options(char *buf);
void easymake_build_project(BuildOptions *build_options);

#endif
