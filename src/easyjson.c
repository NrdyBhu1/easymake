/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 EasySoft
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software
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
#include "easyjson.h"
#include "easylib.h"

/* Function Definitions */
JsonValue *
ezjson_compile(char *json)
{
	String buffer = ez_strtrm(json, " \n");
	int length = ez_strlen(buffer);
	
	JsonValue *container = (JsonValue *)ez_alloc(sizeof(JsonValue));
	
	container->key = NULL;
	container->value_type = JSON_TYPE_OBJECT;
	container->string_value = NULL;
	container->values_count = 0;
	container->values = NULL;
	
	int count = 1;
	JsonValue **values = (JsonValue **)ez_alloc(sizeof(JsonValue *));
	
	values[count - 1] = container;
	
	String storage = (String)ez_alloc(sizeof(char));
	storage[0] = '\0';
	
	int i, j = 0;
	for (i = 0; i < length; i++) {
		char c = buffer[i];
		
		switch (c) {
			case '{':
			case '[': {
				JsonValue *value = (JsonValue *)ez_alloc(sizeof(JsonValue));
				
				if (c == '{') value->value_type = JSON_TYPE_OBJECT;
				if (c == '[') value->value_type = JSON_TYPE_ARRAY;
				
				if (ez_strlen(storage) > 0) {
					value->key = ez_strdup(storage);
					
					storage = (String)ez_realloc(storage, sizeof(char));
					storage[0] = '\0';
				} else {
					value->key = NULL;
				}
				
				value->string_value = NULL;
				value->values_count = 0;
				value->values = NULL;
				
				if (values[count - 1]->values == NULL) {
					values[count - 1]->values = (JsonValue **)ez_alloc(sizeof(JsonValue *));
				} else {
					values[count - 1]->values = (JsonValue **)ez_realloc(values[count - 1]->values, sizeof(JsonValue *) * (values[count - 1]->values_count + 1));
				}
				
				values[count - 1]->values_count++;
				values[count - 1]->values[values[count - 1]->values_count - 1] = value;
				
				values = (JsonValue **)ez_realloc(values, sizeof(JsonValue *) * (count + 1));
				
				count++;
				values[count - 1] = value;
				
				break;
			}
			case ']':
			case '}': {
				values = (JsonValue **)ez_realloc(values, sizeof(JsonValue *) * (count - 1));
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
							
							JsonValue *value = (JsonValue *)ez_alloc(sizeof(JsonValue));
							
							value->value_type = JSON_TYPE_NOVALUE;
							
							if(ez_strlen(storage) > 0) {
								value->key = ez_strdup(storage);
								
								storage = (String)ez_realloc(storage, sizeof(char));
								storage[0] = '\0';
							} else {
								value->key = NULL;
							}
							
							value->string_value = NULL;
							value->values_count = 0;
							value->values = NULL;
							
							if (values[count - 1]->values == NULL) {
								values[count - 1]->values = (JsonValue **)ez_alloc(sizeof(JsonValue *));
							} else {
								values[count - 1]->values = (JsonValue **)ez_realloc(values[count - 1]->values, sizeof(JsonValue *) * (values[count - 1]->values_count + 1));
							}
							
							values[count - 1]->values_count++;
							values[count - 1]->values[values[count - 1]->values_count - 1] = value;
						} else {
							if (i + 2 < length) {
								if (buffer[i + 2] != '{' && buffer[i + 2] != '[') {
									JsonValue *value = (JsonValue *)ez_alloc(sizeof(JsonValue));
									
									value->value_type = JSON_TYPE_STRING;
									
									if(ez_strlen(storage) > 0) {
										value->key = ez_strdup(storage);
										
										storage = (String)ez_realloc(storage, sizeof(char));
										storage[0] = '\0';
									} else {
										value->key = NULL;
									}
									
									value->string_value = NULL;
									value->values_count = 0;
									value->values = NULL;
									
									if (values[count - 1]->values == NULL) {
										values[count - 1]->values = (JsonValue **)ez_alloc(sizeof(JsonValue *));
									} else {
										values[count - 1]->values = (JsonValue **)ez_realloc(values[count - 1]->values, sizeof(JsonValue *) * (values[count - 1]->values_count + 1));
									}
									
									values[count - 1]->values_count++;
									values[count - 1]->values[values[count - 1]->values_count - 1] = value;
									
									values = (JsonValue **)ez_realloc(values, sizeof(JsonValue *) * (count + 1));
									
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
					if (ez_strlen(storage) > 0) {
						values[count - 1]->string_value = ez_strdup(storage);
						
						storage = (String)ez_realloc(storage, sizeof(char));
						storage[0] = '\0';
					} else {
						values[count - 1]->string_value = NULL;
					}
					
					values = (JsonValue **)ez_realloc(values, sizeof(JsonValue *) * (count - 1));
					count--;
					
					j = 0;
				}
				break;
			}
			default: {
				if(j == 1 || j == 4) {
					int storage_length = ez_strlen(storage) + 1;
					storage = (String)ez_realloc(storage, sizeof(char) * (storage_length + 1));
					
					storage[storage_length - 1] = c;
					storage[storage_length] = '\0';
				}
				break;
			}
		}
	}
	
	ez_free(buffer);
	ez_free(storage);
	
	values = ez_realloc(values, 1);
	ez_free(values);
	
	return container;
}

char *
ezjson_decompile(JsonValue *value)
{
	/*
	 * This function needs to take in a JsonValue
	 * and basically branch through it and generate
	 * clean json.
	 */
	 
	 if (value->value_type != JSON_TYPE_OBJECT) {
	 	return NULL;
	 }
	 
	 int i;
	 for (i = 0; i < value->values_count; i++) {
	 	
	 }
}

void
ezjson_free(JsonValue *value)
{
	/*
	 * This function just needs to branch
	 * through the JsonValue and free everything
	 * that is allocated with ez_alloc. I'll implement
	 * this function unless you really want to do it.
	 */
}
