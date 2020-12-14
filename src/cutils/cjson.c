/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Cleanware
 *
 * Permission is hereby granted, c_free of charge, to any person obtaining a copy of this software
 * and associated documentation files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 * BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/* Includes */
#include "cjson.h"
#include "cmemory.h"
#include "cstring.h"

/* Function Definitions */
JsonValue *
cjson_parse(char *json)
{
	String buffer = cstr_ftrim(json, " \n");
	int length = cstr_length(buffer);

	JsonValue *container = (JsonValue *)c_alloc(sizeof(JsonValue));

	container->key = NULL;
	container->value_type = JSON_TYPE_OBJECT;
	container->string_value = NULL;
	container->values_count = 0;
	container->values = NULL;

	int count = 1;
	JsonValue **values = (JsonValue **)c_alloc(sizeof(JsonValue *));

	values[count - 1] = container;

	String storage = (String)c_alloc(sizeof(char));
	storage[0] = '\0';

	int i, j = 0;
	for (i = 0; i < length; i++) {
		char c = buffer[i];

		switch (c) {
			case '{':
			case '[': {
				JsonValue *value = (JsonValue *)c_alloc(sizeof(JsonValue));

				if (c == '{') value->value_type = JSON_TYPE_OBJECT;
				if (c == '[') value->value_type = JSON_TYPE_ARRAY;

				if (cstr_length(storage) > 0) {
					value->key = cstr_dupe(storage);

					storage = (String)c_realloc(storage, sizeof(char));
					storage[0] = '\0';
				} else {
					value->key = NULL;
				}

				value->string_value = NULL;
				value->values_count = 0;
				value->values = NULL;

				if (values[count - 1]->values == NULL) {
					values[count - 1]->values = (JsonValue **)c_alloc(sizeof(JsonValue *));
				} else {
					values[count - 1]->values = (JsonValue **)c_realloc(values[count - 1]->values, sizeof(JsonValue *) * (values[count - 1]->values_count + 1));
				}

				values[count - 1]->values_count++;
				values[count - 1]->values[values[count - 1]->values_count - 1] = value;

				values = (JsonValue **)c_realloc(values, sizeof(JsonValue *) * (count + 1));

				count++;
				values[count - 1] = value;

				break;
			}
			case ']':
			case '}': {
				values = (JsonValue **)c_realloc(values, sizeof(JsonValue *) * (count - 1));
				count--;

				break;
			}
			case '\"':
			case '\'': {
				if (j == 0) {
					j = 1;
				} else if (j == 1) {
					if (i + 1 < length) {
						if (buffer[i + 1] != ':') {
							j = 0;

							JsonValue *value = (JsonValue *)c_alloc(sizeof(JsonValue));

							value->value_type = JSON_TYPE_NOVALUE;

							if(cstr_length(storage) > 0) {
								value->key = cstr_dupe(storage);

								storage = (String)c_realloc(storage, sizeof(char));
								storage[0] = '\0';
							} else {
								value->key = NULL;
							}

							value->string_value = NULL;
							value->values_count = 0;
							value->values = NULL;

							if (values[count - 1]->values == NULL) {
								values[count - 1]->values = (JsonValue **)c_alloc(sizeof(JsonValue *));
							} else {
								values[count - 1]->values = (JsonValue **)c_realloc(values[count - 1]->values, sizeof(JsonValue *) * (values[count - 1]->values_count + 1));
							}

							values[count - 1]->values_count++;
							values[count - 1]->values[values[count - 1]->values_count - 1] = value;
						} else {
							if (i + 2 < length) {
								if (buffer[i + 2] != '{' && buffer[i + 2] != '[') {
									JsonValue *value = (JsonValue *)c_alloc(sizeof(JsonValue));

									value->value_type = JSON_TYPE_STRING;

									if(cstr_length(storage) > 0) {
										value->key = cstr_dupe(storage);

										storage = (String)c_realloc(storage, sizeof(char));
										storage[0] = '\0';
									} else {
										value->key = NULL;
									}

									value->string_value = NULL;
									value->values_count = 0;
									value->values = NULL;

									if (values[count - 1]->values == NULL) {
										values[count - 1]->values = (JsonValue **)c_alloc(sizeof(JsonValue *));
									} else {
										values[count - 1]->values = (JsonValue **)c_realloc(values[count - 1]->values, sizeof(JsonValue *) * (values[count - 1]->values_count + 1));
									}

									values[count - 1]->values_count++;
									values[count - 1]->values[values[count - 1]->values_count - 1] = value;

									values = (JsonValue **)c_realloc(values, sizeof(JsonValue *) * (count + 1));

									count++;
									values[count - 1] = value;

									j = 3;
									i++;
								} else {
									j = 0;
									i++;
								}
							}
						}
					}
				} else if (j == 3) {
					j = 4;
				} else if (j == 4) {
					if (cstr_length(storage) > 0) {
						values[count - 1]->string_value = cstr_dupe(storage);

						storage = (String)c_realloc(storage, sizeof(char));
						storage[0] = '\0';
					} else {
						values[count - 1]->string_value = NULL;
					}

					values = (JsonValue **)c_realloc(values, sizeof(JsonValue *) * (count - 1));
					count--;

					j = 0;
				}
				break;
			}
			default: {
				if(j == 1 || j == 4) {
					int storage_length = cstr_length(storage) + 1;
					storage = (String)c_realloc(storage, sizeof(char) * (storage_length + 1));

					storage[storage_length - 1] = c;
					storage[storage_length] = '\0';
				}
				break;
			}
		}
	}

	c_free(buffer);
	c_free(storage);

	values = c_realloc(values, 1);
	c_free(values);

	return container;
}

void
gen_json(char **json, int index, JsonValue *value)
{
	if (value->value_type == JSON_TYPE_OBJECT || value->value_type == JSON_TYPE_ARRAY) {
		int i;
		for (i = 0; i < index; i++) *json = cstr_fconcat(*json, "	");

		if (value->key != NULL) {
			*json = cstr_fconcat(*json, "\"");
			*json = cstr_fconcat(*json, value->key);
			*json = cstr_fconcat(*json, "\": ");
		}

		if (value->value_type == JSON_TYPE_OBJECT) *json = cstr_fconcat(*json, "{\n");
		else if (value->value_type == JSON_TYPE_ARRAY) *json = cstr_fconcat(*json, "[\n");

		for (i = 0; i < value->values_count; i++) {
			JsonValue *arrval = value->values[i];

			gen_json(json, index + 1, arrval);

			if (i + 1 < value->values_count) {
				*json = cstr_fconcat(*json, ",\n");
			} else {
				*json = cstr_fconcat(*json, "\n");
			}
		}

		for (i = 0; i < index; i++) *json = cstr_fconcat(*json, "	");

		if (value->value_type == JSON_TYPE_OBJECT) *json = cstr_fconcat(*json, "}");
		else if (value->value_type == JSON_TYPE_ARRAY) *json = cstr_fconcat(*json, "]");
	} else {
		if (value->value_type == JSON_TYPE_STRING) {
			int i;
			for (i = 0; i < index; i++) *json = cstr_fconcat(*json, "	");

			if (value->key != NULL) {
				*json = cstr_fconcat(*json, "\"");
				*json = cstr_fconcat(*json, value->key);
				*json = cstr_fconcat(*json, "\": \"");
			}

			if (value->string_value != NULL) {
				*json = cstr_fconcat(*json, value->string_value);
			}

			*json = cstr_fconcat(*json, "\"");
		} else if (value->value_type == JSON_TYPE_NOVALUE) {
			int i;
			for (i = 0; i < index; i++) *json = cstr_fconcat(*json, "	");

			if (value->key != NULL) {
				*json = cstr_fconcat(*json, "\"");
				*json = cstr_fconcat(*json, value->key);
				*json = cstr_fconcat(*json, "\"");
			}
		}
	}
}

char *
cjson_generate(JsonValue *value)
{
	char *json = (char *)c_alloc(sizeof(char));
	json[0] = '\0';

	gen_json(&json, 0, value);

	return json;
}

void
cjson_c_free(JsonValue *value)
{
	if (value->values != NULL) {
		int i;
		for (i = 0; i < value->values_count; i++) {
			JsonValue *arrval = value->values[i];

			if (arrval != NULL) {
				ezjson_c_free(arrval);
			}
		}

		c_free(value->values);
	}

	if (value->key != NULL) {
		c_free(value->key);
	}

	if (value->string_value != NULL) {
		c_free(value->string_value);
	}

	c_free(value);
}
