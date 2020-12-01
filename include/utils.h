#ifndef UTILS_H
#define UTILS_H

static char *concat(const char *a, const char *b)
{
  char *result = (char *)malloc(strlen(a) + strlen(b) + 1);
  
  strcpy(result, a);
  strcat(result, b);
  
  return result;
}

static char *cstrdup(const char *s)
{
    size_t size = strlen(s) + 1;
    char *p = malloc(size);
    if (p != NULL) {
        memcpy(p, s, size);
    }
    return p;
}

static char *cstrndup(const char *s, size_t n)
{
    char *p;
    size_t n1;

    for (n1 = 0; n1 < n && s[n1] != '\0'; n1++)
        continue;
    p = malloc(n + 1);
    if (p != NULL) {
        memcpy(p, s, n1);
        p[n1] = '\0';
    }
    return p;
}

#endif
