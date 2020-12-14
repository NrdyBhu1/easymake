/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Cleanware
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

#include "cstring.h"
#include "cmemory.h"

int
cstr_length(String str)
{
	int length;
	for (length = 0; str[length] != '\0'; length++);

	return length;
}

int
cstr_compare(String a, String b)
{
	int length = cstr_length(a);

	if (length != cstr_length(b)) {
		return 0;
	}

	int i;
	for (i = 0; i < length; i++) {
		if (a[i] != b[i]) {
			return 0;
		}
	}

	return 1;
}

void
cstr_copy(String dest, String src)
{
	c_memcpy(dest, src, cstr_length(src));
}

String
cstr_dupe(String str)
{
	int length = cstr_length(str);

	String new_str = (String)c_alloc(length + 1);
	c_memcpy(new_str, str, length + 1);

	return new_str;
}

String
cstr_concat(String str, String cat)
{
	int str_length = cstr_length(str), cat_length = cstr_length(cat);
	String new_str = (String)c_alloc(str_length + cat_length + 1);

	c_memcpy(new_str, str, str_length);
	c_memcpy(new_str + str_length, cat, cat_length + 1);

	return new_str;
}

String
cstr_fconcat(String str, String cat)
{
	int str_length = cstr_length(str), cat_length = cstr_length(cat);
	String new_str = (String)c_alloc(str_length + cat_length + 1);

	c_memcpy(new_str, str, str_length);
	c_memcpy(new_str + str_length, cat, cat_length + 1);

	c_free(str);
	c_free(cat);

	return new_str;
}

String
cstr_cut(String str, int index)
{
	String new_str = (String)c_alloc(index + 1);
	c_memcpy(new_str, str, index);

	new_str[index] = '\0';

	return new_str;
}

String
cstr_fcut(String str, int index)
{
	String new_str = (String)c_alloc(index + 1);
	c_memcpy(new_str, str, index);

	new_str[index] = '\0';

	c_free(str);

	return new_str;
}

String
cstr_trim(String str, String trim)
{
	String new_str = (String)c_alloc(sizeof(char));
	new_str[0] = '\0';

	int i;
	for (i = 0; str[i] != '\0'; i++) {
		int j, skip = 0;
		for (j = 0; trim[j] != '\0'; j++) {
			if (str[i] == trim[j]) {
				skip = 1;
				break;
			}
		}

		if (!skip) {
			int length = cstr_length(new_str);
			new_str = (String)c_realloc(new_str, length + 2);

			new_str[length] = str[i];
			new_str[length + 1] = '\0';
		}
	}

	return new_str;
}

String
cstr_ftrim(String str, String trim)
{
	String new_str = (String)c_alloc(sizeof(char));
	new_str[0] = '\0';

	int i;
	for (i = 0; str[i] != '\0'; i++) {
		int j, skip = 0;
		for (j = 0; trim[j] != '\0'; j++) {
			if (str[i] == trim[j]) {
				skip = 1;
				break;
			}
		}

		if (!skip) {
			int length = cstr_length(new_str);
			new_str = (String)c_realloc(new_str, length + 2);

			new_str[length] = str[i];
			new_str[length + 1] = '\0';
		}
	}

	c_free(str);
	c_free(trim);

	return new_str;
}
