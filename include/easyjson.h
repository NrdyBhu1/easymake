#ifndef EASYJSON_H
#define EASYJSON_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

enum json_type { json_type_unknown, json_type_string, json_type_number, json_type_array, json_type_object };

struct json_value { int type; char *key; long length; };
struct json_string { int type; char *key; long length; char *string; };
struct json_number { int type; char *key; long length; char *number; };
struct json_object { int type; char *key; long length; struct json_value **values; };

static struct json_object *json_parse(char *buffer)
{
  long buffer_length = strlen(buffer);
  
  int current_count = 1;
  struct json_value **current = (struct json_value **)malloc(sizeof(struct json_value *));
  
  char *storage = (char *)malloc(sizeof(char) * 1);
  storage[0] = '\0';
  
  struct json_object *object = (struct json_object *)malloc(sizeof(struct json_object));
  
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
      {
        printf("start of new object\n");
        
        current = (struct json_value **)realloc(current, current_count + 1);
        
        int l;
        for(l = current_count; l > 0; l--)
        {
          current[l] = current[l - 1];
        }
        
        current[0] = (struct json_value *)malloc(sizeof(struct json_object));
        
        current[0]->type = json_type_object;
        
        current[0]->length = 0;
        ((struct json_object *)current[0])->values = NULL;
        
        if(strlen(storage) > 0)
        {
          current[0]->key = strdup(storage);
          storage = (char *)realloc(storage, 0);
        }
        
        if(((struct json_object *)current[1])->length > 0)
        {
          ((struct json_object *)current[1])->values = (struct json_value **)realloc(((struct json_object *)current[1])->values, ((struct json_object *)current[1])->length + 1);
          ((struct json_object *)current[1])->values[((struct json_object *)current[1])->length - 1] = current[0];
          ((struct json_object *)current[1])->length++;
        }
        else
        {
          ((struct json_object *)current[1])->values = (struct json_value **)malloc(sizeof(struct json_object *));
          ((struct json_object *)current[1])->values[((struct json_object *)current[1])->length - 1] = current[0];
          ((struct json_object *)current[1])->length++;
        }
        
        break;
      }
      case '\"':
      case '\'':
      {
        printf("skip\n");
        continue;
        break;
      }
      default:
      {
        printf("append\n");
        if(c != ' ')
        {
          if(k == 0)
          {
            int length = strlen(storage);
            char *mod = (char *)malloc(length + 2);
            
            if(mod != NULL)
            {
              strcpy(mod, storage);
              
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
  free(object->values);
}

#endif
