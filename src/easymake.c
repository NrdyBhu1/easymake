#include "easymake.h"
#include "jsmn.h"
#include "utils.h"

float VERSION = 0.1;

int jsoneq(const char *json, jsmntok_t *tok, const char *s) {
  if (tok->type == JSMN_STRING && (int)strlen(s) == tok->end - tok->start &&
      strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
    return 0;
  }
  return -1;
}

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
  package.targets_count = 1;
  
  BuildTarget **targets = (BuildTarget **)malloc(sizeof(BuildTarget *) * package.targets_count);
  BuildTarget *current_target = (BuildTarget *)malloc(sizeof(BuildTarget));
  
  current_target->target = "default";
  
  targets[0] = current_target;

  jsmn_parser parser;
  jsmntok_t tokens[512];
  jsmn_init(&parser);
  int i;
  int r = jsmn_parse(&parser, buf, strlen(buf), tokens, sizeof(tokens) / sizeof(tokens[0]));

  if (r > 1 && tokens[0].type == JSMN_OBJECT)
  {
    for (i = 1; i < r; i++)
    {
      if (jsoneq(buf, &tokens[i], "targets") == 0)
      {
        if (tokens[i + 1].type != JSMN_ARRAY) {
          printf("easymake: targets must be a group\n");
          continue;
        }
        
        targets = (BuildTarget **)realloc(targets, sizeof(BuildTarget *) * tokens[i + 1].size);
      }
      
      else if (jsoneq(buf, &tokens[i], "target"))
      {
        current_target->target = cstrndup(buf + tokens[i + 1].start, tokens[i + 1].end - tokens[i + 1].start);
        i++;
      }
      
      else if (jsoneq(buf, &tokens[i], "{"))
      {
        current_target = (BuildTarget *)malloc(sizeof(BuildTarget));
      }
      
      else if (jsoneq(buf, &tokens[i], "}"))
      {
        targets[package.targets_count] = current_target;
        package.targets_count++;
        
        current_target = targets[0];
      }
      
      else if (jsoneq(buf, &tokens[i], "project") == 0)
      {
        current_target->boptions.project = cstrndup(buf + tokens[i + 1].start, tokens[i + 1].end - tokens[i + 1].start);
        i++;
      }

      else if (jsoneq(buf, &tokens[i], "output") == 0)
      {
        current_target->boptions.output = cstrndup(buf + tokens[i + 1].start, tokens[i + 1].end - tokens[i + 1].start);
        i++;
      }

      else if (jsoneq(buf, &tokens[i], "compiler") == 0)
      {
        current_target->boptions.compiler = cstrndup(buf + tokens[i + 1].start, tokens[i + 1].end - tokens[i + 1].start);
        i++;
      }

      else if (jsoneq(buf, &tokens[i], "includes") == 0)
      {
        int j;

        if (tokens[i + 1].type != JSMN_ARRAY) {
          printf("easymake: includes must be a group\n");
          continue;
        }

        char **includes = (char **)malloc(sizeof(char *) * tokens[i + 1].size);

        for (j = 0; j < tokens[i + 1].size; j++)
        {
          jsmntok_t *g = &tokens[i + j + 2];
          includes[j] = cstrndup(buf + g->start, g->end - g->start);
        }

        current_target->boptions.includes = (const char **)includes;
        current_target->boptions.includes_count = tokens[i + 1].size;

        i += tokens[i + 1].size + 1;
      }

      else if (jsoneq(buf, &tokens[i], "sources") == 0)
      {
        int j;

        if (tokens[i + 1].type != JSMN_ARRAY) {
          printf("easymake: sources must be a group\n");
          continue;
        }

        char **sources = (char **)malloc(sizeof(char *) * tokens[i + 1].size);

        for (j = 0; j < tokens[i + 1].size; j++)
        {
          jsmntok_t *g = &tokens[i + j + 2];
          sources[j] = cstrndup(buf + g->start, g->end - g->start);
        }

        current_target->boptions.sources = (const char **)sources;
        current_target->boptions.sources_count = tokens[i + 1].size;

        i += tokens[i + 1].size + 1;
      }

      else if (jsoneq(buf, &tokens[i], "compiler_options") == 0)
      {
        int j;

        if (tokens[i + 1].type != JSMN_ARRAY) {
          printf("easymake: compiler_options must be a group\n");
          continue;
        }

        char **compiler_options = (char **)malloc(sizeof(char *) * tokens[i + 1].size);

        for (j = 0; j < tokens[i + 1].size; j++)
        {
          jsmntok_t *g = &tokens[i + j + 2];
          compiler_options[j] = cstrndup(buf + g->start, g->end - g->start);
        }

        current_target->boptions.compiler_options = (const char **)compiler_options;
        current_target->boptions.compiler_options_count = tokens[i + 1].size;

        i += tokens[i + 1].size + 1;
      }

      else
      {
        printf("easymake: unexpected key: %.*s\n", tokens[i].end - tokens[i].start,
               buf + tokens[i].start);
      }
    }
  }
  else
  {
    printf("easymake: invalid json!\n");
  }
  
  package.targets = targets;

  free(buf);
  return package;
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
      printf("%s\n", target->target);
      if (strcmp(target->target, target_name) == 0)
      {
        printf("easymake: building project \'%s\' using compiler \'%s\'\n", target->boptions.project, target->boptions.compiler);
        
        char command[512] = "";
        char *temp = "";
        
        if(!target->boptions.compiler)
        {
          printf("easymake: no compiler specified\n");
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
      
        if(target->boptions.sources)
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
          return;
        }
      
        if(target->boptions.includes)
        for(i = 0; i < target->boptions.includes_count; i++)
        {
          temp = concat(command, " -I");
          strcpy(command, temp);
          free(temp);
      
          temp = concat(command, (target->boptions.includes)[i]);
          strcpy(command, temp);
          free(temp);
        }
      
        if(target->boptions.libraries)
        for(i = 0; i < target->boptions.libraries_count; i++)
        {
          temp = concat(command, " ");
          strcpy(command, temp);
          free(temp);
      
          temp = concat(command, (target->boptions.libraries)[i]);
          strcpy(command, temp);
          free(temp);
        }
      
        if(target->boptions.compiler_options)
        for(i = 0; i < target->boptions.compiler_options_count; i++)
        {
          temp = concat(command, " ");
          strcpy(command, temp);
          free(temp);
      
          temp = concat(command, (target->boptions.compiler_options)[i]);
          strcpy(command, temp);
          free(temp);
        }
      
        system(command);
        printf("easymake: %s\n", command);
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

  return 0;
}
