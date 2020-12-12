#ifndef EASYMAKE_H
#define EASYMAKE_H

struct build_options {
	char *project, *compiler, *output, *default_target;
	char **sources, **includes, **libraries, **compiler_options;
	int sources_count, includes_count, libraries_count, compiler_options_count;
};
typedef struct build_options BuildOptions;

char *easymake_read_file(char *);
BuildOptions easymake_build_options(char *, char *);
void easymake_build_project(BuildOptions *);

#endif
