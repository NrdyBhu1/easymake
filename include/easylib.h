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

#ifndef NULL
  #define NULL (void *)0
#endif

#ifndef HEAP_SIZE
  #define HEAP_SIZE (1024 * 1024)
#endif

/* Types Definitions */
typedef char * string_t;

#ifdef EASYLIB_STDDEFS
  typedef enum { false, true } bool;
  typedef char int8_t;
  typedef unsigned char uint8_t;
  typedef short int16_t;
  typedef unsigned short uint16_t;
  typedef int int32_t;
  typedef unsigned int uint32_t;
  typedef long int64_t;
  typedef unsigned long uint64_t;
#endif

struct block { int size, free; struct block *next; };

unsigned char memory[HEAP_SIZE] = {0};
static struct block *free_blocks = (void *)memory;

static int onheap(void *ptr)
{
  if(((void *)memory) <= ptr && ptr <= ((void *)memory) + HEAP_SIZE) return 1;
  else return 0;
}

static void split(struct block *block, int size)
{
  struct block *new_block = (void *)((void *)block + size + sizeof(struct block));
  
  new_block->size = (block->size) - size - sizeof(struct block);
  new_block->free = 1;
  new_block->next = block->next;
  
  block->size = size;
  block->free = 0;
  block->next = new_block;
}

static void *malloc(int size)
{
  struct block *current, *previous;
  void *result;
  
  if(free_blocks->size == 0)
  {
    free_blocks->size = HEAP_SIZE - sizeof(struct block);
    free_blocks->free = 1;
    free_blocks->next = NULL;
  }
  
  current = free_blocks;
  
  while(current->size < size || current->free == 0 && current->next != NULL)
  {
    previous = current;
    current = current->next;
  }
  
  if(current->size == size)
  {
    current->free = 0;
    result = (void *)(++current);
  }
  else if(current->size > size + sizeof(struct block))
  {
    split(current, size);
    result = (void *)(++current);
  }
  else result = NULL;
  
  return result;
}

static void merge(void)
{
  struct block *cur = free_blocks;
  
  while(cur && cur->next)
  {
    if(cur->free && cur->next->free)
    {
      cur->size += (cur->next->size) + sizeof(struct block);
      cur->next = cur->next->next;
      continue;
    }
    
    cur = cur->next;
  }
}

static void free(void *ptr)
{
  if(onheap(ptr))
  {
    struct block *cur = ptr;
    --cur;
    
    cur->free = 1;
    
    merge();
  }
}

static int isfree(void *ptr)
{
  if(onheap(ptr))
  {
    struct block *cur = ptr;
    cur--;
    
    return cur->free;
  }
  else return 0;
}

static int memsize(void *ptr)
{
  if(onheap(ptr))
  {
    struct block *cur = ptr;
    cur--;
    
    return cur->size;
  }
  else return 0;
}

static void memcpy(void *dest, void *src, int size)
{
  unsigned char *b_dest = (unsigned char *)dest;
  unsigned char *b_src = (unsigned char *)src;
  
  int i;
  for(i = 0; i < size; i++) b_dest[i] = b_src[i];
}
#include <stdio.h>
static void *realloc(void *ptr, int size)
{
  if(onheap(ptr))
  {
    void *new_ptr = malloc(size);
    
    if(new_ptr != NULL)
    {
      memcpy(new_ptr, ptr, memsize(ptr));
      
      free(ptr);
      
      return new_ptr;
    }
    else { printf("newptr is null!\n"); return NULL; }
  }
  else { printf("not on heap!\n"); return NULL; }
}

static int memuse(void)
{
  struct block *current, *previous;
  int result = 0;
  
  if(free_blocks->size == 0)
  {
    free_blocks->size = HEAP_SIZE - sizeof(struct block);
    free_blocks->free = 1;
    free_blocks->next = NULL;
  }
  
  current = free_blocks;
  
  while(current->next != NULL)
  {
    if(!current->free) result++;
    
    previous = current;
    current = current->next;
  }
  
  return result;
}

static int strlen(string_t str)
{
  int length;
  for(length = 0; str[length] != '\0'; length++);
  
  return length;
}

static int strcmp(string_t a, string_t b)
{
  int a_length = strlen(a), b_length = strlen(b);
  
  if(a_length != b_length) return 0;
  
  int i;
  for(i = 0; i < a_length; i++) if(a[i] != b[i]) return 0;
  
  return 1;
}

static void strcpy(string_t dest, string_t src)
{
  memcpy(dest, src, strlen(src));
}

static string_t strdup(string_t str)
{
  int length = strlen(str);
  
  string_t new_str = malloc(length + 1);
  memcpy(new_str, str, length);
  new_str[length] = '\0';
  
  return new_str;
}

static string_t strcat(string_t str, string_t cat)
{
  int str_length = strlen(str), cat_length = strlen(cat);
  string_t new_str = malloc(str_length + cat_length + 1);
  
  memcpy(new_str, str, str_length);
  memcpy(new_str + str_length, cat, cat_length);
  
  new_str[str_length + cat_length] = '\0';
  
  if(onheap(str)) free(str);
  if(onheap(cat)) free(cat);
  
  return new_str;
}

static string_t strcut(string_t str, int index)
{
  string_t new_str = malloc(index + 1);
  memcpy(new_str, str, index);
  
  new_str[index] = '\0';
  
  return new_str;
}

static string_t *strspl(string_t str, int index)
{
  int length = strlen(str);
  
  string_t first_str = malloc(index + 1);
  memcpy(first_str, str, index);
  
  first_str[index] = '\0';
  
  int second_length = length - index;
  
  string_t second_str = malloc(second_length + 1);
  memcpy(second_str, str + index, second_length);
  
  second_str[second_length] = '\0';
  
  string_t *strarr = malloc(sizeof(string_t) * 2);
  
  strarr[0] = first_str;
  strarr[1] = second_str;
  
  return strarr;
}

#endif
