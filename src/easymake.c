#include "easymake.h"
#include "json.h"

float VERSION = 0.1;

char *easymake_read_file(char *file_path)
{
  FILE *file;
  file = fopen(file_path, "rb");
  
  if(!file)
  {
    printf("easymake: file not found!\n");
    return NULL;
  }
  
  fseek(file, 0, SEEK_END);
  
  long length = ftell(file);
  rewind(file);
  
  char *text = calloc(1, length + 1);
  
  if(!text)
  {
    fclose(file);
    
    printf("easymake: memory error\n");
    return NULL;
  }
  
  if(!fread(text, length, 1, file))
  {
    free(text);
    fclose(file);
    
    printf("easymake: failed to read build file!\n");
    return NULL;
  }
  
  fclose(file);
  return text;
}

BuildOptions easymake_build_options(char *buf)
{
  BuildOptions boptions;
  
  // @TODO parse build file and genereate build options
  struct json_value_s *root = json_parse(buf, strlen(buf));
  
  if(root->type == json_type_object)
  {
    struct json_object_s *object = (struct json_object_s *)root;
    
    struct json_object_element_s *key = object->start;
    
    while(key != NULL)
    {
      struct json_string_s *str = key->name;
      if(strcmp(str->string, "project"))
      {
        struct json_value_s *val = key->value;
        
        if(val->type == json_type_string)
        {
          struct json_string_s *strval = (struct json_string_s *)val;
          boptions.project = strval->string;
        }
      }
      else if(strcmp(str->string, "compiler"))
      {
        struct json_value_s *val = key->value;
        
        if(val->type == json_type_string)
        {
          struct json_string_s *strval = (struct json_string_s *)val;
          boptions.compiler = strval->string;
        }
      }
      else if(strcmp(str->string, "output"))
      {
        struct json_value_s *val = key->value;
        
        if(val->type == json_type_string)
        {
          struct json_string_s *strval = (struct json_string_s *)val;
          boptions.output = strval->string;
        }
      }
      else if(strcmp(str->string, "includes"))
      {
        struct json_value_s *val = key->value;
        
        if(val->type == json_type_array)
        {
          struct json_array_s *array = (struct json_array_s *)val;
          
          char **includes = (char **)malloc(sizeof(char *) * array->length);
          
        }
      }
    }
  }
  else
  {
    printf("easymake: invalid json!\n");
  }
  
  free(buf);
  return boptions;
}

void easymake_build_project(BuildOptions *boptions)
{
  // @TODO run build process based on build options
}

int main(int argc, char *argv[])
{
  if(argc > 1)
  {
    // @TODO why isnt this string comparison working??
    if(strcmp(argv[1], "-version"))
    {
      printf("easymake v%.1f - by undersquire\n", VERSION);
    }
    else
    {
      char *buf = easymake_read_file(argv[1]);
      
      if(!buf) return 0;
      
      BuildOptions boptions = easymake_build_options(buf);
      easymake_build_project(&boptions);
    }
  }
  else
  {
    char *buf = easymake_read_file("build.ezmk");
    
    if(!buf) return 0;
    
    BuildOptions boptions = easymake_build_options(buf);
    easymake_build_project(&boptions);
  }
  
  return 0;
}
