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

#include "cmemory.h"

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
c_alloc(int size)
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
c_realloc(void *ptr, int size)
{
	if (c_onheap(ptr)) {
		void *new_ptr = c_alloc(size);

		if (new_ptr) {
			c_memcpy(new_ptr, ptr, size);

			c_free(ptr);

			return new_ptr;
		} else {
			return NULL;
		}
	} else {
		return NULL;
	}
}

void
c_free(void *ptr)
{
	if (c_onheap(ptr)) {
		Block *current = ptr;
		current--;

		current->free = 1;

		merge();
	}
}

int
c_memsize(void *ptr)
{
	if (c_onheap(ptr)) {
		Block *current = ptr;
		current--;

		return current->size;
	} else {
		return -1;
	}
}

void
c_memcpy(void *dest, void *src, int size)
{
	unsigned char *b_dest = (unsigned char *)dest;
	unsigned char *b_src = (unsigned char *)src;

	int i;
	for (i = 0; i < size; i++) {
		b_dest[i] = b_src[i];
	}
}

int
c_onheap(void *ptr)
{
	return ((void *)HEAP) <= ptr && ptr <= (((void *)HEAP) + HEAP_SIZE);
}

int
c_memuse(void)
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
c_talloc(void)
{
	return TOTAL_ALLOCATIONS;
}
