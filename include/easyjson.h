#ifndef EASYJSON_H
#define EASYJSON_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

enum json_type { json_type_unknown, json_type_string, json_type_number, json_type_array, json_type_object };

typedef struct json_value { int type; char *key; int length; } json_value;
typedef struct json_string { int type; char *key; int length; char *value; } json_string;
typedef struct json_object { int type; char *key; int length; json_value **values; } json_object;

static struct json_object *json_parse(char *json)
{
  long json_length = strlen(json);
  char *buffer = calloc(1, sizeof(char));
  buffer[0] = '\0';

  int b;
  for(b = 0; b < json_length; b++)
  {
    char c = json[b];

    if(c != ' ')
    {
      int length = strlen(buffer);
      char *mod = calloc(length + 2, sizeof(char));

      if(mod != NULL)
      {
        if(length > 0) memcpy(mod, buffer, length);

        mod[length] = c;
        mod[length + 1] = '\0';

        free(buffer);

        buffer = mod;
      }
    }
  }

  long buffer_length = strlen(buffer);

  int current_count = 1;
  json_value **current = calloc(1, sizeof(json_value *));

  char *storage = calloc(1, sizeof(char));
  storage[0] = '\0';

  json_object *object = calloc(1, sizeof(json_object));

  object->type = json_type_object;
  object->key = "easyjson";
  object->length = 0;
  object->values = NULL;

  current[0] = (json_value *)object;

  int i, j = 0, k2 = 0;
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
        current = realloc(current, sizeof(json_value *) * (current_count + 1));
        current_count++;

        int l;
        for(l = current_count - 1; l > 0; l--)
        {
          current[l] = current[l - 1];
        }

        current[0] = malloc(sizeof(json_object));

        if(c == '{') current[0]->type = json_type_object;
        else if(c == '[') current[0]->type = json_type_array;

        current[0]->length = 0;
        ((json_object *)current[0])->values = NULL;

        if(strlen(storage) > 0)
        {
          current[0]->key = strdup(storage);

          storage = realloc(storage, sizeof(char));
          storage[0] = '\0';
        }
        else
        {
          current[0]->key = "";
        }

        if(current[1]->length > 0)
        {
          ((json_object *)current[1])->values = realloc(((json_object *)current[1])->values, sizeof(json_object *) * (current[1]->length + 1));
          current[1]->length++;

          ((json_object *)current[1])->values[current[1]->length - 1] = current[0];
        }
        else
        {
          ((json_object *)current[1])->values = calloc(1, sizeof(json_object *));
          current[1]->length++;

          ((json_object *)current[1])->values[current[1]->length - 1] = current[0];
        }

        break;
      }
      case ']':
      case '}':
      {
        if(current_count > 1)
        {
          int l;
          for(l = 0; l < current_count - 1; l++)
          {
            current[l] = current[l + 1];
          }

          current = realloc(current, sizeof(json_value *) * (current_count - 1));
          current_count--;
        }
        break;
      }
      case '\"':
      case '\'':
      {
        if(k2 == 0) k2 = 1;
        else if(k2 == 1)
        {
          if(buffer[i + 1] != ':')
          {
            k2 = 0;
            struct json_value *jval = calloc(1, sizeof(json_value));

            jval->type = json_type_unknown;

            if(strlen(storage) > 0)
            {
              jval->key = strdup(storage);

              storage = realloc(storage, sizeof(char));
              storage[0] = '\0';
            }
            else
            {
              jval->key = "";
            }

            jval->length = 0;

            if(current[0]->length > 0)
            {
              ((json_object *)current[0])->values = realloc(((json_object *)current[0])->values, sizeof(json_value *) * (current[0]->length + 1));
              current[0]->length++;

              ((json_object *)current[0])->values[current[0]->length - 1] = jval;
            }
            else
            {
              ((json_object *)current[0])->values = calloc(1, sizeof(json_value *));
              current[0]->length++;

              ((json_object *)current[0])->values[current[0]->length - 1] = jval;
            }
          }
          else
          {
            if(buffer[i + 2] != '{' && buffer[i + 2] != '[')
            {
              current = realloc(current, sizeof(json_value *) * (current_count + 1));
              current_count++;

              int l;
              for(l = current_count - 1; l > 0; l--)
              {
                current[l] = current[l - 1];
              }

              current[0] = calloc(1, sizeof(json_string));

              current[0]->type = json_type_string;

              if(strlen(storage) > 0)
              {
                current[0]->key = strdup(storage);

                storage = realloc(storage, sizeof(char));
                storage[0] = '\0';
              }
              else
              {
                current[0]->key = "";
              }

              current[0]->length = 0;

              if(current[1]->length > 0)
              {
                ((json_object *)current[1])->values = realloc(((json_object *)current[1])->values, sizeof(json_string *) * (current[1]->length + 1));
                current[1]->length++;

                ((json_object *)current[1])->values[current[1]->length - 1] = current[0];
              }
              else
              {
                ((json_object *)current[1])->values = calloc(1, sizeof(json_string *));
                current[1]->length++;

                ((json_object *)current[1])->values[current[1]->length - 1] = current[0];
              }

              k2 = 3;
              i++;
            }
            else
            {
              k2 = 0;
              i++;
            }
          }
        }
        else if(k2 == 3)
        {
          k2 = 4;
        }
        else if(k2 == 4)
        {
          if(strlen(storage) > 0)
          {
            ((json_string *)current[0])->value = strdup(storage);

            storage = realloc(storage, sizeof(char));
            storage[0] = '\0';
          }

          if(current_count > 1 && current[1]->length > 0)
          {
            int l;
            for(l = 0; l < current_count - 1; l++)
            {
              current[l] = current[l + 1];
            }

            current = realloc(current, sizeof(json_value *) * (current_count - 1));
            current_count--;
          }

          k2 = 0;
        }
        break;
      }
      default:
      {
        if(c != ' ')
        {
          if(k2 == 1 || k2 == 4)
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
        }
        break;
      }
    }
  }

  free(buffer);
  free(storage);

  current = realloc(current, 0);
  free(current);

  return object;
}

void json_free(json_object *object)
{
  int i;
  for(i = 0; i < object->length; i++)
  {
    if(object->values[i]->type == json_type_object
      || object->values[i]->type == json_type_array)
        json_free((json_object *)object->values[i]);
    else
    {
      free(object->values[i]->key);
      free(object->values[i]);
    }
  }
  free(object->values);
  free(object);
}

#endif
