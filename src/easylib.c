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

#include "easylib.h"

/* Types */
typedef struct block {
	int size, free;
	struct block *next;
} Block;

/* Function Declarations */
void split(Block *, int);
void merge(void);

/* Global Variables */
unsigned char HEAP[HEAP_SIZE];
Block *FREE_BLOCKS = (void *)HEAP;
int TOTAL_ALLOCATIONS = 0;

/* Function Definitions */
void
split(Block *block, int size)
{
	Block *new_block = (Block *)(block + size + sizeof(Block));
	
	new_block->size = block->size - size - sizeof(Block);
	new_block->free = 1;
	new_block->next = block->next;
	
	block->size = size;
	block->free = 0;
	block->next = new_block;
}

void
merge(void)
{
	if (FREE_BLOCKS->size == 0) return;
	
	Block *current = FREE_BLOCKS;
	
	while (current && current->next) {
		if (current->free && current->next->free) {
			current->size += current->next->size + sizeof(Block);
			current->next = current->next->next;
			
			continue;
		}
		
		current = current->next;
	}
}

void *
ez_alloc(int size)
{
	Block *current = FREE_BLOCKS;
	
	if (FREE_BLOCKS->size == 0) {
		FREE_BLOCKS->size = HEAP_SIZE - sizeof(Block);
		FREE_BLOCKS->free = 1;
		FREE_BLOCKS->next = NULL;
	}
	
	TOTAL_ALLOCATIONS += size;
	
	while (current != NULL) {
		if (current->free) {
			if (current->size == size) {
				current->free = 0;
				current++;
				
				return (void *)current;
			} else if (current->size > size + sizeof(Block)) {
				split(current, size);
				current++;
				
				return (void *)current;
			}
		}
		
		current = current->next;
	}
	
	return NULL;
}

void *
ez_realloc(void *ptr, int size)
{
	if (ez_onheap(ptr)) {
		void *new_ptr = ez_alloc(size);
		
		if (new_ptr) {
			ez_memcpy(new_ptr, ptr, size);
			
			ez_free(ptr);
			
			return new_ptr;
		} else {
			return NULL;
		}
	} else {
		return NULL;
	}
}

void
ez_free(void *ptr)
{
	if (ez_onheap(ptr)) {
		Block *current = ptr;
		current--;
		
		current->free = 1;
		
		merge();
	}
}

int
ez_memsize(void *ptr)
{
	if (ez_onheap(ptr)) {
		Block *current = ptr;
		current--;
		
		return current->size;
	} else {
		return -1;
	}
}

void
ez_memcpy(void *dest, void *src, int size)
{
	unsigned char *b_dest = (unsigned char *)dest;
	unsigned char *b_src = (unsigned char *)src;
	
	int i;
	for (i = 0; i < size; i++) {
		b_dest[i] = b_src[i];
	}
}

int
ez_memuse(void)
{
	if (FREE_BLOCKS->size == 0) {
		return 0;
	}
	
	Block *current = FREE_BLOCKS;
	int result = 0;
	
	while (current != NULL) {
		if (!current->free) {
			result += current->size;
		}
		
		current = current->next;
	}
	
	return result;
}

int
ez_onheap(void *ptr)
{
	return ((void *)HEAP) <= ptr && ptr <= (((void *)HEAP) + HEAP_SIZE);
}

int
ez_talloc(void)
{
	return TOTAL_ALLOCATIONS;
}

int
ez_strlen(String str)
{
	int length;
	for (length = 0; str[length] != '\0'; length++);
	
	return length;
}

int
ez_strcmp(String a, String b)
{
	int length = ez_strlen(a);
	
	if (length != ez_strlen(b)) {
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
ez_strcpy(String dest, String src)
{
	ez_memcpy(dest, src, ez_strlen(src));
}

String
ez_strdup(String str)
{
	int length = ez_strlen(str);
	
	String new_str = (String)ez_alloc(length + 1);
	ez_memcpy(new_str, str, length + 1);
	
	return new_str;
}

String
ez_strcat(String str, String cat)
{
	int str_length = ez_strlen(str), cat_length = ez_strlen(cat);
	String new_str = (String)ez_alloc(str_length + cat_length + 1);
	
	ez_memcpy(new_str, str, str_length);
	ez_memcpy(new_str + str_length, cat, cat_length + 1);
	
	return new_str;
}

String
ez_strcut(String str, int index)
{
	String new_str = (String)ez_alloc(index + 1);
	ez_memcpy(new_str, str, index);
	
	new_str[index] = '\0';
	
	return new_str;
}

String
ez_strtrm(String str, String trim)
{
	String new_str = (String)ez_alloc(sizeof(char));
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
			int length = ez_strlen(new_str);
			new_str = (String)ez_realloc(new_str, length + 2);
			
			new_str[length] = str[i];
			new_str[length + 1] = '\0';
		}
	}
	
	return new_str;
}
