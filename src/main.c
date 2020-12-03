#include "easyjson.h"
#include <stdio.h>

int main(void)
{
  char *json = "{ \"test\":\"hello\" }";
  
  struct json_object *obj = json_parse(json);
  
  printf("Length: %d\n", obj->length);
  
  json_free(obj);
}
