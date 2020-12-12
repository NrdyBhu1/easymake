#include "easymake.h"
#include "easylib.h"
#include "easyjson.h"

#include <stdlib.h>
#include <stdio.h>

float VERSION = 0.2;
int EXIT_CODE = 0, EXIT_AFTER_PREP_BLDOPTS = 0;

char *
easymake_read_file(char *file_path)
{
	FILE *file;
	file = fopen(file_path, "r");
	
	if (!file) {
		printf("easymake: error: build file not found\n");
		return NULL;
	}
	
	fseek(file, 0, SEEK_END);
	
	long length = ftell(file);
	rewind(file);
	
	char *text = (char *)ez_alloc(length + 1);
	
	size_t read_count = fread(text, 1, length, file);
	text[read_count] = '\0';
	
	fclose(file);
	return text;
}

void
apply_build_options(BuildOptions *boptions, JsonValue *object)
{
	int i;
	for (i = 0; i < object->values_count; i++) {
		JsonValue *val = object->values[i];
		
		if (ez_strcmp(val->key, "project")) {
			if (val->value_type == JSON_TYPE_STRING) {
				boptions->project = ez_strdup(val->string_value);
			}
		} else if (ez_strcmp(val->key, "compiler")) {
			if (val->value_type == JSON_TYPE_STRING) {
				boptions->compiler = ez_strdup(val->string_value);
			}
		} else if (ez_strcmp(val->key, "output")) {
			if (val->value_type == JSON_TYPE_STRING) {
				boptions->output = ez_strdup(val->string_value);
			}
		} else if (ez_strcmp(val->key, "default_target")) {
			if(val->value_type == JSON_TYPE_STRING) {
				boptions->default_target = ez_strdup(val->string_value);
			}
		} else if (ez_strcmp(val->key, "sources")) {
			if (val->value_type == JSON_TYPE_ARRAY) {
				boptions->sources = (char **)ez_alloc(sizeof(char *) * val->values_count);
				
				int i;
				for (i = 0; i < val->values_count; i++) {
					boptions->sources[i] = ez_strdup(val->values[i]->key);
				}
				
				boptions->sources_count = val->values_count;
			}
		} else if (ez_strcmp(val->key, "includes")) {
			if (val->value_type == JSON_TYPE_ARRAY) {
				boptions->includes = (char **)ez_alloc(sizeof(char *) * val->values_count);
				
				int i;
				for (i = 0; i < val->values_count; i++) {
					boptions->includes[i] = ez_strdup(val->values[i]->key);
				}
				
				boptions->includes_count = val->values_count;
			}
		} else if (ez_strcmp(val->key, "libraries")) {
			if (val->value_type == JSON_TYPE_ARRAY) {
				boptions->libraries = (char **)ez_alloc(sizeof(char *) * val->values_count);
				
				int i;
				for (i = 0; i < val->values_count; i++) {
					boptions->libraries[i] = ez_strdup(val->values[i]->key);
				}
				
				boptions->libraries_count = val->values_count;
			}
		} else if (ez_strcmp(val->key, "compiler_options")) {
			if (val->value_type == JSON_TYPE_ARRAY) {
				boptions->compiler_options = (char **)ez_alloc(sizeof(char *) * val->values_count);
				
				int i;
				for (i = 0; i < val->values_count; i++) {
					boptions->compiler_options[i] = ez_strdup(val->values[i]->key);
				}
				
				boptions->compiler_options_count = val->values_count;
			}
		}
	}
}

BuildOptions
easymake_build_options(char *buf, char *target)
{
	BuildOptions boptions;
	
	boptions.project = NULL;
	boptions.compiler = NULL;
	boptions.output = NULL;
	boptions.sources = NULL;
	boptions.default_target = NULL;
	boptions.includes = NULL;
	boptions.libraries = NULL;
	boptions.compiler_options = NULL;
	boptions.sources_count = 0;
	boptions.includes_count = 0;
	boptions.libraries_count = 0;
	boptions.compiler_options_count = 0;
	
	JsonValue *json = ezjson_compile(buf);
	
	if (json->values[0]->values_count < 1) {
		printf("easymake: error: invalid build file\n");
		
		EXIT_CODE = 6;
		EXIT_AFTER_PREP_BLDOPTS = 1;
		
		return boptions;
	}
	
	JsonValue *object = json->values[0];
	
	apply_build_options(&boptions, object);
	
	if (target == NULL) {
		target = boptions.default_target;
	}
	
	printf("easymake: building project \'%s\' with target \'%s\'\n", boptions.project, target);
	
	int i;
	for (i = 0; i < object->values_count; i++) {
		JsonValue *val = object->values[i];
		
		if (ez_strcmp(val->key, "targets")) {
			if (val->value_type == JSON_TYPE_ARRAY || val->value_type == JSON_TYPE_OBJECT) {
				int j;
				for (j = 0; j < val->values_count; j++) {
					JsonValue *arrval = val->values[j];
					
					if (ez_strcmp(arrval->key, target)) {
						if (arrval->value_type == JSON_TYPE_OBJECT) {
							apply_build_options(&boptions, val->values[j]);
						}
					}
				}
			}
		}
	}
	
	//ezjson_free(json);
	
	return boptions;
}

void
easymake_build_project(BuildOptions *boptions) {
	if (EXIT_AFTER_PREP_BLDOPTS) return;
	
	char *command = "";
	
	if (!boptions->compiler) {
		printf("easymake: error: no compiler specified\n");
		
		EXIT_CODE = 2;
		return;
	}
	
	command = ez_strcat(command, boptions->compiler);
	
	if (boptions->output) {
		command = ez_strcat(command, " -o ");
		command = ez_strcat(command, boptions->output);
	}
	
	int i;
	
	if (boptions->sources_count > 0) {
		for (i = 0; i < boptions->sources_count; i++) {
			command = ez_strcat(command, " ");
			command = ez_strcat(command, boptions->sources[i]);
		}
	} else {
		printf("easymake: error: no source file(s) specified\n");
		
		EXIT_CODE = 1;
		return;
	}
	
	if (boptions->includes_count > 0) {
		for (i = 0; i < boptions->includes_count; i++) {
			command = ez_strcat(command, " -I");
			command = ez_strcat(command, boptions->includes[i]);
		}
	}
	
	if (boptions->libraries_count > 0) {
		for (i = 0; i < boptions->libraries_count; i++) {
			command = ez_strcat(command, " -L");
			command = ez_strcat(command, boptions->libraries[i]);
		}
	}
	
	if (boptions->compiler_options_count > 0) {
		for (i = 0; i < boptions->compiler_options_count; i++) {
			command = ez_strcat(command, " ");
			command = ez_strcat(command, boptions->compiler_options[i]);
		}
	}
	
	printf("easymake: executing: \'%s\'\n\n", command);
	
	system(command);
	
	printf("\neasymake: build process complete. check above for compiler errors\n");
	
	ez_free(command);
}

int main(int argc, char *argv[])
{
	char *file = "build.ezmk", *target = NULL;
	
	if (argc > 1) {
		int i;
		for (i = 1; i < argc; i++) {
			char *arg = argv[i];
			
			if (ez_strcmp(arg, "-v") || ez_strcmp(arg, "--version")) {
				printf("easymake v%.1f - copyright (c) 2020 - 2021 EasySoft\n", VERSION);
				return 0;
			} else if (ez_strcmp(arg, "-h") || ez_strcmp(arg, "--help")) {
				printf("easymake: usage: $ easymake [options] <target>\n");
				return 0;
			} else if (ez_strcmp(arg, "-f") || ez_strcmp(arg, "--file")) {
				if (i + 1 < argc) {
					file = argv[i + 1];
				}
			} else {
				target = argv[i];
			}
		}
	}
	
	char *buf = easymake_read_file(file);
	
	if (buf) {
		BuildOptions boptions = easymake_build_options(buf, target);
		easymake_build_project(&boptions);
	}
	
	if(EXIT_CODE != 0) {
		printf("easymake: error: non-zero exit code (%d)\n", EXIT_CODE);
	}
	
	return EXIT_CODE;
}
