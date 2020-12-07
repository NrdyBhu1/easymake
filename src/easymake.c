#include "easymake.h"
#include "easylib.h"
#include "easyjson.h"
#include "utils.h"

float VERSION = 0.1;
int EXIT_CODE = 0;

bool EXIT_AFTER_PREP_BLDOPTS = false;

char *easymake_read_file(char *file_path)
{
  FILE *file;
  file = fopen(file_path, "r");

  if(!file)
  {
    printf("easymake: build file not found!\n");
    return NULL;
  }

  fseek(file, 0, SEEK_END);

  long length = ftell(file);
  rewind(file);

  char *text = (char *)malloc(length + 1);

  size_t read_count = fread(text, 1, length, file);
  text[read_count] = '\0';

  fclose(file);
  return text;
}

void apply_build_options(BuildOptions *boptions, json_object *object)
{
  int i;
  for(i = 0; i < object->length; i++)
  {
    json_value *val = object->values[i];
    
    if(strcmp(val->key, "project"))
    {
      if(val->type == json_type_string)
      {
        json_string *str = (json_string *)val;
        boptions->project = strdup(str->value);
      }
    }
    else if(strcmp(val->key, "compiler"))
    {
      if(val->type == json_type_string)
      {
        json_string *str = (json_string *)val;
        boptions->compiler = strdup(str->value);
      }
    }
    else if(strcmp(val->key, "output"))
    {
      if(val->type == json_type_string)
      {
        json_string *str = (json_string *)val;
        boptions->output = strdup(str->value);
      }
    }
    else if(strcmp(val->key, "sources"))
    {
      if(val->type == json_type_array)
      {
        json_object *arr = (json_object *)val;
        
        boptions->sources = malloc(sizeof(char *) * arr->length);
        
        int i;
        for(i = 0; i < arr->length; i++) 
        {
          boptions->sources[i] = strdup(arr->values[i]->key);
        }
        
        boptions->sources_count = arr->length;
      }
    }
    else if(strcmp(val->key, "includes"))
    {
      if(val->type == json_type_array)
      {
        json_object *arr = (json_object *)val;
        
        boptions->includes = malloc(sizeof(char *) * arr->length);
        
        int i;
        for(i = 0; i < arr->length; i++) 
        {
          boptions->includes[i] = strdup(arr->values[i]->key);
        }
        
        boptions->includes_count = arr->length;
      }
    }
    else if(strcmp(val->key, "libraries"))
    {
      if(val->type == json_type_array)
      {
        json_object *arr = (json_object *)val;
        
        boptions->libraries = malloc(sizeof(char *) * arr->length);
        
        int i;
        for(i = 0; i < arr->length; i++) 
        {
          boptions->libraries[i] = strdup(arr->values[i]->key);
        }
        
        boptions->libraries_count = arr->length;
      }
    }
    else if(strcmp(val->key, "compiler_options"))
    {
      if(val->type == json_type_array)
      {
        json_object *arr = (json_object *)val;
        
        boptions->compiler_options = malloc(sizeof(char *) * arr->length);
        
        int i;
        for(i = 0; i < arr->length; i++) 
        {
          boptions->includes[i] = strdup(arr->values[i]->key);
        }
        
        boptions->includes_count = arr->length;
      }
    }
  }
}

BuildOptions easymake_build_options(char *buf, char *target)
{
  BuildOptions boptions;
  
  boptions.project = NULL;
  boptions.compiler = NULL;
  boptions.output = NULL;
  boptions.sources = NULL;
  boptions.includes = NULL;
  boptions.libraries = NULL;
  boptions.compiler_options = NULL;
  boptions.sources_count = 0;
  boptions.includes_count = 0;
  boptions.libraries_count = 0;
  boptions.compiler_options_count = 0;
  
  printf("easymake: parsing json...\n");
  json_object *json = json_parse(buf);
  
  printf("easymake: parsing json data...\n");
  if(json->values[0]->type != json_type_object)
  {
    printf("easymake: invalid buildfile\n");
    
    EXIT_CODE = 6;
    EXIT_AFTER_PREP_BLDOPTS = true;
    
    return boptions;
  }

  json_object *object = (json_object *)json->values[0];
  
  apply_build_options(&boptions, object);
  
  int i;
  for(i = 0; i < object->length; i++)
  {
    json_value *val = object->values[i];
    
    if(strcmp(val->key, "targets"))
    {
      if(val->type == json_type_array)
      {
        json_object *arr = (json_object *)val;
        
        int j;
        for(j = 0; j < arr->length; j++)
        {
          json_value *val = arr->values[j];
          
          if(strcmp(val->key, target))
          {
            if(val->type == json_type_object)
            {
              json_object *obj = (json_object *)arr->values[j];
              apply_build_options(&boptions, obj);
            }
          }
        }
      }
    }
  }
  
  json_free(object);
  
  return boptions;
}

void easymake_build_project(BuildOptions *boptions)
{
  if(EXIT_AFTER_PREP_BLDOPTS) return;
  
  
}

int main(int argc, char *argv[])
{
  char *file = "build.ezmk", *target = "\0";
  
  if(argc > 1)
  {
    int i;
    for(i = 1; i < argc; i++)
    {
      char *arg = argv[i];
      
      if(strcmp(arg, "-v") || strcmp(arg, "--version"))
      {
        printf("easymake v%.1f - copyright (c) 2020 - 2021 EasySoft\n", VERSION);
        return 0;
      }
      else if(strcmp(arg, "-h") || strcmp(arg, "--help"))
      {
        printf("easymake: usage: $ easymake [options] <target>\n");
        return 0;
      }
      else if(strcmp(arg, "-f") || strcmp(arg, "--file"))
      {
        if(i + 1 < argc)
        {
          file = argv[i + 1];
        }
      }
    }
  }
  
  printf("easymake: reading build file...\n");
  char *buf = easymake_read_file(file);
  
  if(buf)
  {
    printf("easymake: building project options...\n");
    BuildOptions boptions = easymake_build_options(buf, target);
    
    printf("easymake: building project...\n");
    easymake_build_project(&boptions);
  }
  
  if(EXIT_CODE != 0) {
    printf("easymake: error: non-zero exit code (%d)\n", EXIT_CODE);
  }

  return EXIT_CODE;
}
