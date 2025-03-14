#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)


int printf(const char *fmt, ...) {
  // conversion specification:
  //  %[$][flags][width][.precision][length modifier]conversion
  panic("not implemented");
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  return vsnprintf(out, 0xffffffff, fmt, ap);
}

int sprintf(char *out, const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  return vsprintf(out, fmt, ap);
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  return vsnprintf(out, n, fmt, ap);
}


/**
 * @brief Write a number to a string buffer
 *
 * @param str
 * @param end
 * @param num
 * @return char* return the terminating null byte
 */
static char* number(char *str, char *end, int64_t num) {
  char sign = '+';
  if (num < 0) {
    sign = '-';
    num = -num;
  }
  char buf[64];
  int i = 0;
  do {
    buf[i++] = num % 10 + '0';
    num /= 10;
  } while (num);
  if (sign == '-') buf[i++] = '-';
  while (i > 0) {
    if (str == end) return str;
    *str++ = buf[--i];
  }
  *str = '\0';
  return str;
}

// for more reference: https://github.com/torvalds/linux/blob/76544811c850a1f4c055aa182b513b7a843868ea/lib/vsprintf.c#L2760
int vsnprintf(char *buf, size_t size, const char *fmt, va_list ap) {
  char *str, *end;
  str = buf;
  end = buf + size;

  /* make sure buf is always smaller than end */
  if (end < buf) {
    end = ((void *)-1);
    size = end - buf;
  }

  while(*fmt) {
    if (*fmt != '%') {
      if (str < end) *str++ = *fmt;
      fmt++;
      continue;
    }
    fmt++;

    switch (*fmt) {
      case 'd': {
        int32_t num = va_arg(ap, int32_t);
        str = number(str, end - 1, num);
        break;
      }
      case 's': {
        char *s = va_arg(ap, char *);
        str = stpcpy(str, s);
        break;
      }
    }

    fmt++;
  }
  *str = '\0';
  return str - buf;
}

#endif