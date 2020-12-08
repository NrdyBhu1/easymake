#include "easyjson.h"
#include <stdio.h>

int main(void)
{
  char *json = "{ \"test\": \"project\" }";
  
  json_object *obj = json_parse(json);
  
  json_free(obj);
}
