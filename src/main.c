#include "easyjson.h"
#include <stdio.h>

void print_tree(struct json_object *obj)
{
  int i;
  for(i = 0; i < obj->length; i++)
  {
    struct json_value *val = obj->values[i];
    if(val->type == json_type_object) print_tree((struct json_object *)val);
    else printf("%s -> %d\n", val->key, val->type);
  }
}

int main(void)
{
  char *json = "{ \"test\":\"hello\", \"test2\":\"goodbye\" }";
  
  struct json_object *obj = json_parse(json);
  
  struct json_object *obj2 = (struct json_object *)obj->values[0];
  
  printf("%d\n", obj2->length);
  
  /* print_tree(obj); */
  
  json_free(obj);
}
