#ifndef UTILS_H
#define UTILS_H

typedef enum { true, false } bool;

static char *concat(char *a, char *b)
{
  char *result = (char *)malloc(strlen(a) + strlen(b) + 1);

  strcpy(result, a);
  strcat(result, b);

  return result;
}

static char *cstrndup(char *s, size_t n)
{
  char *p = malloc(n + 1);
  if(p != NULL)
  {
    memcpy(p, s, n);
    p[n] = '\0';
  }

  return p;
}

#endif
