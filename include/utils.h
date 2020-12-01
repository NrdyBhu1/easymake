#ifndef UTILS_H
#define UTILS_H

static char *concat(const char *a, const char *b)
{
  char *result = (char *)malloc(strlen(a) + strlen(b) + 1);
  
  strcpy(result, a);
  strcat(result, b);
  
  return result;
}

static char *cstrndup(const char *s, size_t n)
{
  char *p = malloc(n + 1);
  if(p != NULL)
  {
    memcpy(p, s, n);
    p[n + 1] = '\0';
  }
  
  return p;  
}

#endif
