#include "easymake.h"
#include "cmemory.h"
#include "cstring.h"
#include "cjson.h"

#include <stdlib.h>
#include <stdio.h>

float VERSION = 0.2;
int EXIT_CODE = 0, SKIP_NEXT_STEP = 0;

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

	char *text = (char *)c_alloc(length + 1);

	size_t read_count = fread(text, 1, length, file);
	text[read_count] = '\0';

	fclose(file);

	/* Build file has to be atleast `{}\0` */
	if(length >= 3) {
		return text;
	} else {
		SKIP_NEXT_STEP = 1;
		return NULL;
	}
}

void
apply_build_options(BuildOptions *boptions, JsonValue *object)
{
	if(SKIP_NEXT_STEP) return;

	int i;
	for (i = 0; i < object->values_count; i++) {
		JsonValue *val = object->values[i];

		if (cstr_compare(val->key, "project")) {
			if (val->value_type == JSON_TYPE_STRING) {
				boptions->project = cstr_dupe(val->string_value);
			}
		} else if (cstr_compare(val->key, "compiler")) {
			if (val->value_type == JSON_TYPE_STRING) {
				boptions->compiler = cstr_dupe(val->string_value);
			}
		} else if (cstr_compare(val->key, "output")) {
			if (val->value_type == JSON_TYPE_STRING) {
				boptions->output = cstr_dupe(val->string_value);
			}
		} else if (cstr_compare(val->key, "default_target")) {
			if(val->value_type == JSON_TYPE_STRING) {
				boptions->default_target = cstr_dupe(val->string_value);
			}
		} else if (cstr_compare(val->key, "sources")) {
			if (val->value_type == JSON_TYPE_ARRAY) {
				boptions->sources = (char **)c_alloc(sizeof(char *) * val->values_count);

				int i;
				for (i = 0; i < val->values_count; i++) {
					boptions->sources[i] = cstr_dupe(val->values[i]->key);
				}

				boptions->sources_count = val->values_count;
			}
		} else if (cstr_compare(val->key, "includes")) {
			if (val->value_type == JSON_TYPE_ARRAY) {
				boptions->includes = (char **)c_alloc(sizeof(char *) * val->values_count);

				int i;
				for (i = 0; i < val->values_count; i++) {
					boptions->includes[i] = cstr_dupe(val->values[i]->key);
				}

				boptions->includes_count = val->values_count;
			}
		} else if (cstr_compare(val->key, "libraries")) {
			if (val->value_type == JSON_TYPE_ARRAY) {
				boptions->libraries = (char **)c_alloc(sizeof(char *) * val->values_count);

				int i;
				for (i = 0; i < val->values_count; i++) {
					boptions->libraries[i] = cstr_dupe(val->values[i]->key);
				}

				boptions->libraries_count = val->values_count;
			}
		} else if (cstr_compare(val->key, "library_dirs")) {
			if (val->value_type == JSON_TYPE_ARRAY) {
				boptions->library_dirs = (char **)c_alloc(sizeof(char *) * val->values_count);

				int i;
				for (i = 0; i < val->values_count; i++) {
					boptions->library_dirs[i] = cstr_dupe(val->values[i]->key);
				}

				boptions->library_dirs_count = val->values_count;
			}
		} else if (cstr_compare(val->key, "compiler_options")) {
			if (val->value_type == JSON_TYPE_ARRAY) {
				boptions->compiler_options = (char **)c_alloc(sizeof(char *) * val->values_count);

				int i;
				for (i = 0; i < val->values_count; i++) {
					boptions->compiler_options[i] = cstr_dupe(val->values[i]->key);
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

	if(SKIP_NEXT_STEP) return boptions;

	boptions.project = NULL;
	boptions.compiler = NULL;
	boptions.output = NULL;
	boptions.sources = NULL;
	boptions.default_target = NULL;
	boptions.includes = NULL;
	boptions.libraries = NULL;
	boptions.library_dirs = NULL;
	boptions.compiler_options = NULL;
	boptions.sources_count = 0;
	boptions.includes_count = 0;
	boptions.libraries_count = 0;
	boptions.library_dirs_count = 0;
	boptions.compiler_options_count = 0;

	JsonValue *json = cjson_parse(buf);

	if (json->values[0]->values_count < 1) {
		printf("easymake: error: invalid build file\n");

		EXIT_CODE = 6;
		SKIP_NEXT_STEP = 1;

		return boptions;
	}

	JsonValue *object = json->values[0];

	apply_build_options(&boptions, object);

	if (target == NULL) {
		target = boptions.default_target;
	}

	if (target == NULL) {
		printf("easymake: error: no target specified\n");
		cjson_free(json);

		return boptions;
	}

	printf("easymake: building project \'%s\' with target \'%s\'\n", boptions.project, target);

	int i;
	for (i = 0; i < object->values_count; i++) {
		JsonValue *val = object->values[i];

		if (cstr_compare(val->key, "targets")) {
			if (val->value_type == JSON_TYPE_ARRAY || val->value_type == JSON_TYPE_OBJECT) {
				int j;
				for (j = 0; j < val->values_count; j++) {
					JsonValue *arrval = val->values[j];

					if (cstr_compare(arrval->key, target)) {
						if (arrval->value_type == JSON_TYPE_OBJECT) {
							apply_build_options(&boptions, val->values[j]);
						}
					}
				}
			}
		}
	}

	cjson_free(json);

	return boptions;
}

void
easymake_build_project(BuildOptions *boptions)
{
	if (SKIP_NEXT_STEP) return;

	char *command = "";

	if (!boptions->compiler) {
		printf("easymake: error: no compiler specified\n");

		EXIT_CODE = 2;
		return;
	}

	command = cstr_fconcat(command, boptions->compiler);

	if (boptions->output) {
		command = cstr_fconcat(command, " -o ");
		command = cstr_fconcat(command, boptions->output);
	}

	int i;

	if (boptions->sources_count > 0) {
		for (i = 0; i < boptions->sources_count; i++) {
			command = cstr_fconcat(command, " ");
			command = cstr_fconcat(command, boptions->sources[i]);
		}
	} else {
		printf("easymake: error: no source file(s) specified\n");

		EXIT_CODE = 1;
		return;
	}

	if (boptions->includes_count > 0) {
		for (i = 0; i < boptions->includes_count; i++) {
			command = cstr_fconcat(command, " -I");
			command = cstr_fconcat(command, boptions->includes[i]);
		}
	}

	if (boptions->library_dirs_count > 0) {
		for (i = 0; i < boptions->library_dirs_count; i++) {
			command = cstr_fconcat(command, " -L");
			command = cstr_fconcat(command, boptions->library_dirs[i]);
		}
	}

	if (boptions->libraries_count > 0) {
		for (i = 0; i < boptions->libraries_count; i++) {
			command = cstr_fconcat(command, " -l");
			command = cstr_fconcat(command, boptions->libraries[i]);
		}
	}

	if (boptions->compiler_options_count > 0) {
		for (i = 0; i < boptions->compiler_options_count; i++) {
			command = cstr_fconcat(command, " ");
			command = cstr_fconcat(command, boptions->compiler_options[i]);
		}
	}

	printf("easymake: executing: \'%s\'\n\n", command);

	system(command);
	c_free(command);

	if (boptions->project != NULL) c_free(boptions->project);
	if (boptions->compiler != NULL) c_free(boptions->compiler);
	if (boptions->output != NULL) c_free(boptions->output);
	if (boptions->default_target != NULL) c_free(boptions->default_target);

	if (boptions->sources != NULL) {
		int i;
		for (i = 0; i < boptions->sources_count; i++) {
			if (boptions->sources[i] != NULL) {
				c_free(boptions->sources[i]);
			}
		}

		c_free(boptions->sources);
	}

	if (boptions->includes != NULL) {
		int i;
		for (i = 0; i < boptions->includes_count; i++) {
			if (boptions->includes[i] != NULL) {
				c_free(boptions->includes[i]);
			}
		}

		c_free(boptions->includes);
	}

	if (boptions->libraries != NULL) {
		int i;
		for (i = 0; i < boptions->libraries_count; i++) {
			if (boptions->libraries[i] != NULL) {
				c_free(boptions->libraries[i]);
			}
		}

		c_free(boptions->libraries);
	}

	if (boptions->library_dirs != NULL) {
		int i;
		for (i = 0; i < boptions->library_dirs_count; i++) {
			if (boptions->library_dirs[i] != NULL) {
				c_free(boptions->library_dirs[i]);
			}
		}

		c_free(boptions->library_dirs);
	}

	if (boptions->compiler_options != NULL) {
		int i;
		for (i = 0; i < boptions->compiler_options_count; i++) {
			if (boptions->compiler_options[i] != NULL) {
				c_free(boptions->compiler_options[i]);
			}
		}

		c_free(boptions->compiler_options);
	}

	printf("\neasymake: build process complete. check above for compiler errors\n");
}

int
parse_user_value(JsonValue *root, String key, int required)
{
	char data[128];
	int awaiting_input = 1;

	while (awaiting_input) {
		if (!required) {
			awaiting_input = 0;
		}

		fgets(data, 128, stdin);

		if (cstr_length(data) > 0) {
			if (data[cstr_length(data) - 1] == '\n') {
				data[cstr_length(data) - 1] = '\0';
			}
		}

		if (cstr_length(data) > 0) {
			awaiting_input = 0;

			JsonValue *value = (JsonValue *)c_alloc(sizeof(JsonValue));

			value->key = cstr_dupe(key);
			value->value_type = JSON_TYPE_STRING;
			value->values_count = 0;
			value->values = NULL;

			if (root->values == NULL) {
				root->values = (JsonValue **)c_alloc(sizeof(JsonValue *));
				root->values_count = 1;
			} else {
				root->values = (JsonValue **)c_realloc(root->values, sizeof(JsonValue *) * (root->values_count + 1));
				root->values_count++;
			}

			int length = cstr_length(data) + 1;
			value->string_value = (String)c_alloc(sizeof(char) * length);

			cstr_copy(value->string_value, data);
			value->string_value[length - 1] = '\0';

			root->values[root->values_count - 1] = value;
		} else {
			if (required) {
				printf("\nplease specify a value, or press CTRL + C to cancel: ");
			} else {
				return 0;
			}
		}
	}

	return 1;
}

int
parse_user_values(JsonValue *root, String key)
{
	char data[128];
	fgets(data, 128, stdin);

	if (cstr_length(data) > 0) {
		if (data[cstr_length(data) - 1] == '\n') {
			data[cstr_length(data) - 1] = '\0';
		}
	}

	if (cstr_length(data) > 0) {
		JsonValue *value = (JsonValue *)c_alloc(sizeof(JsonValue));

		value->key = cstr_dupe(key);
		value->value_type = JSON_TYPE_ARRAY;
		value->string_value = NULL;
		value->values_count = 0;
		value->values = NULL;

		if (root->values == NULL) {
			root->values = (JsonValue **)c_alloc(sizeof(JsonValue *));
			root->values_count = 1;
		} else {
			root->values = (JsonValue **)c_realloc(root->values, sizeof(JsonValue *) * (root->values_count + 1));
			root->values_count++;
		}

		while (cstr_length(data) > 0) {
			JsonValue *arrvalue = (JsonValue *)c_alloc(sizeof(JsonValue));

			arrvalue->value_type = JSON_TYPE_NOVALUE;
			arrvalue->string_value = NULL;
			arrvalue->values_count = 0;
			arrvalue->values = NULL;

			if (value->values == NULL) {
				value->values = (JsonValue **)c_alloc(sizeof(JsonValue *));
				value->values_count = 1;
			} else {
				value->values = (JsonValue **)c_realloc(value->values, sizeof(JsonValue *) * (value->values_count + 1));
				value->values_count++;
			}

			int length = cstr_length(data) + 1;
			arrvalue->key = (String)c_alloc(sizeof(char) * length);

			cstr_copy(arrvalue->key, data);
			arrvalue->key[length - 1] = '\0';

			value->values[value->values_count - 1] = arrvalue;

			fgets(data, 128, stdin);

			if (cstr_length(data) > 0) {
				if (data[cstr_length(data) - 1] == '\n') {
					data[cstr_length(data) - 1] = '\0';
				}
			}
		}

		root->values[root->values_count - 1] = value;
	} else {
		return 0;
	}

	return 1;
}

void
parse_user_options(JsonValue *root)
{
	int current_question = 1;

	while (current_question > 0) {
		switch (current_question) {
			case 1: {
				printf("question: compiler (none): ");
				parse_user_value(root, "compiler", 0);
				current_question++;

				break;
			}
			case 2: {
				printf("question: output (none): ");
				parse_user_value(root, "output", 0);
				current_question++;

				break;
			}
			case 3: {
				printf("question: sources (none):\n");
				parse_user_values(root, "sources");
				current_question++;

				break;
			}
			case 4: {
				printf("question: includes (none):\n");
				parse_user_values(root, "includes");
				current_question++;

				break;
			}
			case 5: {
				printf("question: libraries (none):\n");
				parse_user_values(root, "libraries");
				current_question++;

				break;
			}
			case 6: {
				printf("question: compiler options (none):\n");
				parse_user_values(root, "compiler_options");
				current_question = 0;

				break;
			}
		}
	}
}

int
init(void)
{
	JsonValue *root = (JsonValue *)c_alloc(sizeof(JsonValue));

	root->key = NULL;
	root->value_type = JSON_TYPE_OBJECT;
	root->string_value = NULL;
	root->values_count = 0;
	root->values = NULL;

	printf("question: project name (*): ");
	parse_user_value(root, "project", 1);

	parse_user_options(root);

	printf("question: create target? (y/n): ");
	char result = fgetc(stdin);
	fflush(stdin);

	JsonValue *targets;

	if (result == 'Y' || result == 'y') {
		targets = (JsonValue *)c_alloc(sizeof(JsonValue));

		targets->key = cstr_dupe("targets");
		targets->value_type = JSON_TYPE_OBJECT;
		targets->string_value = NULL;
		targets->values_count = 0;
		targets->values = NULL;

		if (root->values == NULL) {
			root->values = (JsonValue **)c_alloc(sizeof(JsonValue *));
			root->values_count = 1;
		} else {
			root->values = (JsonValue **)c_realloc(root->values, sizeof(JsonValue *) * (root->values_count + 1));
			root->values_count++;
		}

		root->values[root->values_count - 1] = targets;
	}

	while (result == 'Y' || result == 'y') {
		JsonValue *target = (JsonValue *)c_alloc(sizeof(JsonValue));

		printf("question: target name (*): ");

		char data[128];

		while (1) {
			fgets(data, 128, stdin);

			if (cstr_length(data) > 0) {
				if (data[cstr_length(data) - 1] == '\n') {
					data[cstr_length(data) - 1] = '\0';
				}
			}

			if (cstr_length(data) > 0) {
				break;
			} else {
				printf("\nplease specify a value, or press CTRL + C to cancel: ");
			}
		}

		target->key = cstr_dupe(data);
		target->value_type = JSON_TYPE_OBJECT;
		target->string_value = NULL;
		target->values_count = 0;
		target->values = NULL;

		parse_user_options(target);

		if (targets->values == NULL) {
			targets->values = (JsonValue **)c_alloc(sizeof(JsonValue *));
			targets->values_count = 1;
		} else {
			targets->values = (JsonValue **)c_realloc(targets->values, sizeof(JsonValue *) * (targets->values_count + 1));
			targets->values_count++;
		}

		targets->values[targets->values_count - 1] = target;

		printf("question: create target? (y/n): ");
		result = fgetc(stdin);
		fflush(stdin);
	}

	String json = cjson_generate(root);

	FILE *file;

	file = fopen("build.ezmk", "w+");
	fprintf(file, json);
	fclose(file);

	c_free(json);
	cjson_free(root);

	printf("\nsuccessfully genereated build file\n");

	return 0;
}

int main(int argc, char *argv[])
{
	char *file = "build.ezmk", *target = NULL;

	if (argc > 1) {
		int i;
		for (i = 1; i < argc; i++) {
			char *arg = argv[i];

			if (cstr_compare(arg, "-v") || cstr_compare(arg, "--version")) {
				printf("easymake v%.1f - copyright (c) 2020 - 2021 EasySoft\n", VERSION);
				return 0;
			} else if (cstr_compare(arg, "-h") || cstr_compare(arg, "--help")) {
				printf("easymake: usage: $ easymake [options] <target>\n");
				return 0;
			} else if (cstr_compare(arg, "-f") || cstr_compare(arg, "--file")) {
				if (i + 1 < argc) {
					file = argv[i + 1];
				}
			} else if (cstr_compare(arg, "init")) {
				return init();
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
