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

#ifndef EASYJSON_H
#define EASYJSON_H

#ifndef EASYLIB_H
	#error "easyjson: missing dependency \'easylib\'"
#endif

/* Types */
enum {
	JSON_TYPE_NOVALUE,
	JSON_TYPE_STRING,
	JSON_TYPE_OBJECT,
	JSON_TYPE_ARRAY,
};

typedef struct {
	String key;
	int value_type;
	JsonString string_value;
	JsonObject object_value;
} JsonValue;

typedef struct {
	int list_count;
	JsonValue list[100];
} JsonValues;

typedef struct {
	String string_value;
} JsonString;

typedef struct {
	int values_count;
	JsonValues values;
} JsonObject;

/* Function Declarations */
JsonObject *ezjson_parse(String json);
void ezjson_free(JsonObject *object);

#endif
