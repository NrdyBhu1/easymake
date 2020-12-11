#ifndef EASYMAKE_H
#define EASYMAKE_H

#include "easylib.h"
#include <stdio.h>

struct build_options {
  const char *project, *compiler, *output;
  const char **sources, **includes, **libraries, **compiler_options;
  int sources_count, includes_count, libraries_count, compiler_options_count;
};
typedef struct build_options BuildOptions;

char *easymake_read_file(char *);
BuildOptions easymake_build_options(char *, char *);
void easymake_build_project(BuildOptions *);

#endif
