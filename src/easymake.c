#include "easymake.h"
#include "easyjson.h"
#include "utils.h"

float VERSION = 0.1;

int EXIT_CODE = 0;

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

Package easymake_build_options(char *buf)
{
  Package package;
  
  json_object *json = json_parse(buf);
  
  if(json->values[0]->type != json_type_object)
  {
    printf("easymake: invalid buildfile\n");
    EXIT_CODE = 6;
    //return NULL;
  }
  
  json_object *object = (json_object *)json->values[0];
  
  int i;
  for(i = 0; i < object->length; i++)
  {
    
  }
  
  json_free(object);
}

void easymake_build_project(Package *package, char *target_name)
{
  if (!strcmp(target_name, "all"))
  {
    goto cleanup;
  }
  else
  {
    int i;
    for(i = 0; i < package->targets_count; i++)
    {
      BuildTarget *target = package->targets[i];
      if (strcmp(target->target, target_name) == 0)
      {
        printf("easymake: building project \'%s\' with target \'%s\'\n", target->boptions.project, target->target);
        
        char command[512] = "";
        char *temp = "";
        
        if(!target->boptions.compiler)
        {
          printf("easymake: no compiler specified\n");
          EXIT_CODE = 2;
          return;
        }
        
        temp = concat(command, target->boptions.compiler);
        strcpy(command, temp);
        free(temp);
        
        if(target->boptions.output)
        {
          temp = concat(command, " -o ");
          strcpy(command, temp);
          free(temp);
          
          temp = concat(command, target->boptions.output);
          strcpy(command, temp);
          free(temp);
        }
      
        int i;
      
        if(target->boptions.sources_count > 0)
        for(i = 0; i < target->boptions.sources_count; i++)
        {
          temp = concat(command, " ");
          strcpy(command, temp);
          free(temp);
      
          temp = concat(command, (target->boptions.sources)[i]);
          strcpy(command, temp);
          free(temp);
        }
        else
        {
          printf("easymake: no source files specified\n");
          EXIT_CODE = 1;
          return;
        }
      
        if(target->boptions.includes_count > 0)
        for(i = 0; i < target->boptions.includes_count; i++)
        {
          temp = concat(command, " -I");
          strcpy(command, temp);
          free(temp);
      
          temp = concat(command, (target->boptions.includes)[i]);
          strcpy(command, temp);
          free(temp);
        }
                
        if(target->boptions.libraries_count > 0)
        {
          printf("%d\n", target->boptions.libraries_count);
        }
      
        if(target->boptions.libraries_count > 0)
        for(i = 0; i < target->boptions.libraries_count; i++)
        {
          temp = concat(command, " ");
          strcpy(command, temp);
          free(temp);
      
          temp = concat(command, (target->boptions.libraries)[i]);
          strcpy(command, temp);
          free(temp);
        }
      
        if(target->boptions.compiler_options_count > 0)
        for(i = 0; i < target->boptions.compiler_options_count; i++)
        {
          temp = concat(command, " ");
          strcpy(command, temp);
          free(temp);
      
          temp = concat(command, (target->boptions.compiler_options)[i]);
          strcpy(command, temp);
          free(temp);
        }
      
        printf("easymake: %s\n", command);
        system(command);
        
        printf("easymake: build process complete. output file: \'%s\'\n", target->boptions.output);
        
        goto cleanup;
      }
    }
  }
  
  printf("easymake: target not found\n");
  
  cleanup:
  {
    
  }
}

int main(int argc, char *argv[])
{
  if(argc > 1)
  {
    if(!strcmp(argv[1], "-v") || !strcmp(argv[1], "--version"))
    {
      printf("easymake v%.1f - made by the easymake team (all contributors on github)\n", VERSION);
    }
    else if(!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help"))
    {
      printf("easymake v%.1f - usage\n * --help     |  shows this page\n * --version  |  shows the easymake version", VERSION);
    }
    else
    {
      char *buf = easymake_read_file(argv[1]);

      if(!buf) return 0;

      Package package = easymake_build_options(buf);
      easymake_build_project(&package, "default");
    }
  }
  else
  {
    char *buf = easymake_read_file("build.ezmk");

    if(!buf) return 0;

    Package package = easymake_build_options(buf);
    easymake_build_project(&package, "default");
  }

  if(EXIT_CODE != 0) {
    printf("easymake: ERROR: NON-ZERO EXIT CODE %d\n", EXIT_CODE);
  }

  return EXIT_CODE;
}
