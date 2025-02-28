#include <klib.h>
#include <klib-macros.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
  size_t len = 0;
  while (s[len] != '\0') {
    len++;
  }
  return len;
}

// copy src to dst, return dst.
char *strcpy(char *dst, const char *src) {
  size_t len = strlen(src);
  memcpy(dst, src, len);
  dst[len] = '\0';
  return dst;
}

// copy src to dst, return the terminating null byte
char *stpcpy(char *dst, const char *src) {
  size_t len = strlen(src);
  memcpy(dst, src, len);
  dst[len] = '\0';
  return dst + len;
}

char *strncpy(char *dst, const char *src, size_t n) {
  size_t slen = strlen(src);
  memcpy(dst, src, MIN(n, slen));
  for (int i = n; i < slen; i++) {
    dst[i] = '\0';
  }
  return dst;
}

char *strcat(char *dst, const char *src) {
  memcpy(dst + strlen(dst), src, strlen(src) + 1);
  return dst;
}

int strcmp(const char *s1, const char *s2) {
  while (*s1 != '\0' && *s1 == *s2) {
    s1++;
    s2++;
  }
  return *s1 - *s2;
}

int strncmp(const char *s1, const char *s2, size_t n) {
  for (size_t i = 0; i < n; i++) {
    if (s1[i] != s2[i]) return s1[i] - s2[i];
  }
  return 0;
}

void *memset(void *s, int c, size_t n) {
  uint8_t *ps = (uint8_t *)s;
  uint8_t val = (uint8_t)c;
  for(size_t i = 0; i < n; i++) {
    ps[i] = val;
  }
  return s;
}

/// memmove: could overlap
void *memmove(void *dst, const void *src, size_t n) {
  uint8_t *pt = (uint8_t *)dst;
  uint8_t *ps = (uint8_t *)src;
  if (pt > ps) {
    for (size_t i = n - 1; i >= 0; i--) {
      pt[i] = ps[i];
    }
  } else {
    for (size_t i = 0; i < n; i++) {
      pt[i] = ps[i];
    }
  }
  return dst;
}

/// memcpy: should not overlap
void *memcpy(void *out, const void *in, size_t n) {
  uint8_t *pd = (uint8_t *)out;
  uint8_t *ps = (uint8_t *)in;
  for (size_t i = 0; i < n; i++) {
    pd[i] = ps[i];
  }
  return out;
}


int memcmp(const void *s1, const void *s2, size_t n) {
  const uint8_t *p1 = (uint8_t *)s1;
  const uint8_t *p2 = (uint8_t *)s2;
  for (size_t i = 0; i < n; i++) {
    if (p1[i] != p2[i]) return p1[i] - p2[i];
  }
  return 0;
}

#endif
