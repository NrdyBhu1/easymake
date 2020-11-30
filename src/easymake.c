#include "easymake.h"

float VERSION = 0.1;

char *easymake_read_file(char *file)
{
  FILE *file;
  file = fopen(file, "rb");
  
  if(!file)
  {
    prinf("easymake: file not found!\n");
    return;
  }
  
  fseek(file, 0, SEEK_END);
  
  long length = ftell(file);
  rewind(file);
  
  char *text = calloc(1, length + 1);
  
  if(!text)
  {
    fclose(file);
    
    printf("easymake: memory error\n");
    return;
  }
  
  if(!fread(text, length, 1, file))
  {
    free(text);
    fclose(file);
    
    printf("easymake: failed to read build file!\n");
    return 0;
  }
  
  fclose(file);
  return text;
}

BuildOptions easymake_build_options(char *buf)
{
  BuildOptions boptions;
  
  // @TODO parse build file and genereate build options
  
  free(buf);
  return boptions;
}

void easymake_build_project(BuildOptions *boptions)
{
  // @TODO run build process based on build options
}

int main(int argc, char **argv)
{
  if(argc > 1)
  {
    // lol
    easymake_build_project(&easymake_build_options(easymake_read_file(argv[1]));
  }
  else
  {
    printf("easymake v%.1f - by undersquire\n", VERSION);
  }
  
  return 0;
}
