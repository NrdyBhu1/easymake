#include "easyjson.h"
#include <stdio.h>

void print_tree(struct json_object *obj)
{
  int i;
  for(i = 0; i < obj->length; i++)
  {
    struct json_value *val = obj->values[i];
    if(val->type == json_type_object
    || val->type == json_type_array)
    {
      struct json_object *object = (struct json_object *)val;
      printf("json_object: %s -> %d\n", object->key, object->length);
      
      print_tree(object);
    }
    else
    {
      if(val->type == json_type_string)
      {
        struct json_string *str = (struct json_string *)val;
        printf("json_string: %s -> %s\n", str->key, str->value);
      }
      else printf("json_key: %s\n", val->key);
    }
  }
}

int main(void)
{
  char *json = "{ \"project\":\"easymake\", \"targets\":[ \"debug\": { \"compiler\": \"gcc\", \"compiler_options\": [ \"-s\", \"-O3\" ] } ] }";
  
  struct json_object *obj = json_parse(json);
  
  print_tree(obj);
  
  json_free(obj);
}
