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

#ifndef EASYLIB_H
#define EASYLIB_H

#define HEAP_SIZE (1024 * 1024) /* 1 MB */

#ifndef NULL
	#define NULL (void *)0
#endif

#define ANSI_COLOR_WHITE            "\x1b[97m"
#define ANSI_COLOR_BLACK            "\x1b[30m"
#define ANSI_COLOR_RED              "\x1b[31m"
#define ANSI_COLOR_GREEN            "\x1b[32m"
#define ANSI_COLOR_YELLOW           "\x1b[33m"
#define ANSI_COLOR_BLUE             "\x1b[34m"
#define ANSI_COLOR_MAGENTA          "\x1b[35m"
#define ANSI_COLOR_CYAN             "\x1b[36m"

#define ANSI_COLOR_DARK_WHITE       "\x1b[37m"
#define ANSI_COLOR_BRIGHT_BLACK     "\x1b[90m"
#define ANSI_COLOR_BRIGHT_RED       "\x1b[91m"
#define ANSI_COLOR_BRIGHT_GREEN     "\x1b[92m"
#define ANSI_COLOR_BRIGHT_YELLOW    "\x1b[93m"
#define ANSI_COLOR_BRIGHT_BLUE      "\x1b[94m"
#define ANSI_COLOR_BRIGHT_MAGENTA   "\x1b[95m"
#define ANSI_COLOR_BRIGHT_CYAN      "\x1b[96m"

#define ANSI_COLOR_RESET            "\x1b[0m"

/* Types */
typedef char * String;

/* Function Declarations */
void *ez_alloc(int);
void *ez_realloc(void *, int);
void ez_free(void *);
int ez_memsize(void *);
void ez_memcpy(void *, void *, int);
int ez_memuse(void);
int ez_onheap(void *);
int ez_strlen(String);
int ez_strcmp(String, String);
void ez_strcpy(String, String);
String ez_strdup(String);
String ez_strcat(String, String);
String ez_strcut(String, int);
String ez_strtrm(String, String);

#endif
