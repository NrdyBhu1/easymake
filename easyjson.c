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
JsonValue
ezjson_compile(char *json)
{
	String buffer = ez_strtrm(json, " \n");
	int length = ez_strlen(buffer);
	
	int count = 1;
	JsonValue *values = (JsonValue *)ez_alloc(sizeof(JsonValue));
	
	values[count - 1].key = NULL;
	values[count - 1].value_type = JSON_TYPE_OBJECT;
	values[count - 1].string_value = NULL;
	values[count - 1].values_count = 0;
	values[count - 1].values = NULL;
	
	int i;
	for (i = 0; i < length; i++) {
		char c = buffer[i];
		
		switch (c) {
			case '{':
			case '[': {
				JsonValue value;
				
				if (c == '{') value.value_type = JSON_TYPE_OBJECT;
				if (c == '[') value.value_type = JSON_TYPE_ARRAY;
				
				if (values[count - 1].values == NULL) {
					values[count - 1].values = (JsonValue *)ez_alloc(sizeof(JsonValue));
				} else {
					values[count - 1].values = (JsonValue *)ez_realloc(values[count - 1].values, sizeof(JsonValue) * (values[count - 1].values_count + 1));
				}
				
				values[count - 1].values_count++;
				values[count - 1].values[values[count - 1].values_count - 1] = value;
				
				values = ez_realloc(values, sizeof(JsonValue) * (count + 1));
				values[count - 1] = value;
				
				break;
			}
			case ']':
			case '}': {
				values = ez_realloc(values, sizeof(JsonValue) * (count - 1));
				
			}
		}
	}
}
