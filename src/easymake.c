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

BuildOptions easymake_build_options(char *buf)
{
  BuildOptions boptions;

  jsmn_parser parser;
  jsmntok_t tokens[512];
  jsmn_init(&parser);
  int i;
  int r = jsmn_parse(&parser, buf, strlen(buf), tokens, sizeof(tokens) / sizeof(tokens[0]));

  if(r > 1 && tokens[0].type == JSMN_OBJECT)
  {
    for (i = 1; i < r; i++)
    {
      if (jsoneq(buf, &tokens[i], "project") == 0)
      {
        printf("- Project: %.*s\n", tokens[i + 1].end - tokens[i + 1].start,
               buf + tokens[i + 1].start);

        boptions.project = cstrndup(buf + tokens[i + 1].start, tokens[i + 1].end - tokens[i + 1].start);
        i++;
      }

      else if (jsoneq(buf, &tokens[i], "output") == 0)
      {
        printf("- Output: %.*s\n", tokens[i + 1].end - tokens[i + 1].start,
               buf + tokens[i + 1].start);

        boptions.output = cstrndup(buf + tokens[i + 1].start, tokens[i + 1].end - tokens[i + 1].start);
        i++;
      }

      else if (jsoneq(buf, &tokens[i], "compiler") == 0)
      {
        printf("- Compiler: %.*s\n", tokens[i + 1].end - tokens[i + 1].start,
               buf + tokens[i + 1].start);

        boptions.compiler = cstrndup(buf + tokens[i + 1].start, tokens[i + 1].end - tokens[i + 1].start);
        i++;
      }
 
      else if (jsoneq(buf, &tokens[i], "includes") == 0)
      {
        int j;
        printf("- Includes:\n");
        
        if (tokens[i + 1].type != JSMN_ARRAY) {
          continue;
        }

        char **includes = (char **)malloc(sizeof(char *) * tokens[i + 1].size);

        for (j = 0; j < tokens[i + 1].size; j++)
        {
          jsmntok_t *g = &tokens[i + j + 2];
          printf("  * %.*s\n", g->end - g->start, buf + g->start);
          includes[j] = cstrndup(buf + g->start, g->end - g->start);
        }

        boptions.includes = (const char **)includes;
        boptions.includes_count = tokens[i + 1].size;
        
        i += tokens[i + 1].size + 1;
      }

      else if (jsoneq(buf, &tokens[i], "sources") == 0)
      {
        int j;
        printf("- Sources:\n");
        
        if (tokens[i + 1].type != JSMN_ARRAY) {
          continue;
        }

        char **sources = (char **)malloc(sizeof(char *) * tokens[i + 1].size);

        for (j = 0; j < tokens[i + 1].size; j++)
        {
          jsmntok_t *g = &tokens[i + j + 2];
          printf("  * %.*s\n", g->end - g->start, buf + g->start);
          sources[j] = cstrndup(buf + g->start, g->end - g->start);
        }

        boptions.sources = (const char **)sources;
        boptions.sources_count = tokens[i + 1].size;
        
        i += tokens[i + 1].size + 1;
      }

      else
      {
        printf("Unexpected key: %.*s\n", tokens[i].end - tokens[i].start,
               buf + tokens[i].start);
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
  printf("easymake: building project \'%s\' using compiler \'%s\'\n", boptions->project, boptions->compiler);
  
  char command[256];
  char *temp = "";
  
  if(!boptions->compiler)
  {
    printf("easymake: no compiler specified!\n");
    return;
  }
  
  temp = concat(command, boptions->compiler);
  strcpy(command, temp);
  free(temp);
  
  if(boptions->output)
  {
    temp = concat(command, " -o ");
    strcpy(command, temp);
    free(temp);
    
    temp = concat(command, boptions->output);
    strcpy(command, temp);
    free(temp);
  }
  
  int i;
  
  if(boptions->sources)
  for(i = 0; i < boptions->sources_count; i++)
  {
    temp = concat(command, " ");
    strcpy(command, temp);
    free(temp);
    
    temp = concat(command, (boptions->sources)[i]);
    strcpy(command, temp);
    free(temp);
  }
  else
  {
    printf("easymake: no source files specified!\n");
    return;
  }
  
  if(boptions->includes)
  for(i = 0; i < boptions->includes_count; i++)
  {
    temp = concat(command, " -I");
    strcpy(command, temp);
    free(temp);
    
    temp = concat(command, (boptions->includes)[i]);
    strcpy(command, temp);
    free(temp);
  }
  
  if(boptions->libraries)
  for(i = 0; i < boptions->libraries_count; i++)
  {
    temp = concat(command, " ");
    strcpy(command, temp);
    free(temp);
    
    temp = concat(command, (boptions->libraries)[i]);
    strcpy(command, temp);
    free(temp);
  }
  
  if(boptions->compiler_options)
  for(i = 0; i < boptions->compiler_options_count; i++)
  {
    temp = concat(command, " ");
    strcpy(command, temp);
    free(temp);
    
    temp = concat(command, (boptions->compiler_options)[i]);
    strcpy(command, temp);
    free(temp);
  }
  
  system(command);
  
  printf("easymake: build process complete. output file: \'%s\'\n", boptions->output);
}

int main(int argc, char *argv[])
{
  if(argc > 1)
  {
    if(!strcmp(argv[1], "-version"))
    {
      printf("easymake v%.1f - by undersquire\n", VERSION);
    }
    else
    {
      char *buf = easymake_read_file(argv[1]);
      
      if(!buf) return 0;
      
      BuildOptions boptions = easymake_build_options(buf);
     // easymake_build_project(&boptions);
    }
  }
  else
  {
    char *buf = easymake_read_file("build.ezmk");
    
    if(!buf) return 0;
    
    BuildOptions boptions = easymake_build_options(buf);
    //easymake_build_project(&boptions);
  }
  
  return 0;
}
