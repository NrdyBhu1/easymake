#ifndef EASYJSON_H
#define EASYJSON_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

enum json_type { json_type_unknown, json_type_string, json_type_number, json_type_array, json_type_object };

struct json_value { int type; char *key; int length; };
struct json_string { int type; char *key; int length; char *string; };
struct json_number { int type; char *key; int length; char *number; };
struct json_object { int type; char *key; int length; struct json_value **values; };

static struct json_object *json_parse(char *buffer)
{
  long buffer_length = strlen(buffer);
  
  int current_count = 1;
  struct json_value **current = calloc(1, sizeof(struct json_value *));
  
  char *storage = calloc(1, sizeof(char));
  storage[0] = '\0';
  
  struct json_object *object = calloc(1, sizeof(struct json_object));
  
  object->type = json_type_object;
  object->key = "easyjson";
  object->length = 0;
  object->values = NULL;
  
  current[0] = (struct json_value *)object;
  
  int i, j = 0, k = 0;
  for(i = 0; i < buffer_length; i++)
  {
    char c = buffer[i];
    
    if(j > 1)
    {
      if(c == '\n') j = 0;
      continue;
    }
    if(c == '/') j++; else j = 0;
    
    switch(c)
    {
      case '{':
      case '[':
      {
        current = realloc(current, sizeof(struct json_value *) * (current_count + 1));
        
        int l;
        for(l = current_count; l > 0; l--)
        {
          current[l] = current[l - 1];
        }
        
        current[0] = malloc(sizeof(struct json_object));
        
        current[0]->type = json_type_object;
        
        current[0]->length = 0;
        ((struct json_object *)current[0])->values = NULL;
        
        if(strlen(storage) > 0)
        {
          current[0]->key = strdup(storage);
          
          storage = realloc(storage, sizeof(char));
          storage[0] = '\0';
        }
        
        if(current[1]->length > 0)
        {
          ((struct json_object *)current[1])->values = realloc(((struct json_object *)current[1])->values, sizeof(struct json_object *) * (current[1]->length + 1));
          current[1]->length++;
          
          ((struct json_object *)current[1])->values[current[1]->length - 1] = current[0];
        }
        else
        {
          ((struct json_object *)current[1])->values = calloc(1, sizeof(struct json_object *));
          current[1]->length++;
          
          ((struct json_object *)current[1])->values[current[1]->length - 1] = current[0];
        }
        
        break;
      }
      case ']':
      case '}':
      {
        
        break;
      }
      case ':':
      {
        k = 1;
        break;
      }
      case '\"':
      case '\'':
      {
        switch(k)
        {
          case 0:
          {
            continue;
            break;
          }
          case 1:
          {
            
            break;
          }
          case 2:
          {
            
            break;
          }
        }
        break;
      }
      default:
      {
        if(c != ' ')
        {
          if(k == 0)
          {
            int length = strlen(storage);
            char *mod = calloc(length + 2, sizeof(char));
            
            if(mod != NULL)
            {
              if(length > 0) memcpy(mod, storage, length);
              
              mod[length] = c;
              mod[length + 1] = '\0';
              
              free(storage);
              
              storage = mod;
            }
            else
            {
              printf("easyjson: malloc failure\n");
            }
          }
          else
          {
            
          }
        }
        break;
      }
    }
  }
  
  return object;
}

void json_free(struct json_object *object)
{
  int i;
  for(i = 0; i < object->length; i++)
  {
    free(object->values[i]);
  }
  free(object->values);
}

#endif
