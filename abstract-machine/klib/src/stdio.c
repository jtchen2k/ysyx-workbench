#include <am.h>
#include <klib-macros.h>
#include <klib.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

#define SIGN 1 // signed / unsigend
#define PLUS 2 // show plus
#define SMALL 4

enum format_state {
  FORMAT_STATE_NONE,
  FORMAT_STATE_NUM,
  FORMAT_STATE_STR,
  FORMAT_STATE_CHAR,
  FORMAT_STATE_INVALID,
  FORMAT_STATE_PERCENT_CHAR,
};

struct fmt {
  const char   *str;
  unsigned char state; // enum format_state
  unsigned char size;  // size of numbers
};

struct printf_spec {
  unsigned char flags;       /* flags to number() */
  unsigned char base;        /* number base, 8, 10 or 16 only */
  short         precision;   /* # of digits/chars */
  int           field_width; /* width of output field */
} __packed;

/**
 * Helper function to decode printf style format.
 * Each call decode a token from the format and return the
 * number of characters read.
 */
static __attribute__((noinline)) struct fmt
format_decode(struct fmt fmt, struct printf_spec *spec) {
  const char *start = fmt.str;
  fmt.state = FORMAT_STATE_NONE;

  for (; *fmt.str; fmt.str++) {
    if (*fmt.str == '%')
      break;
  }

  // return current non-format string
  if (fmt.str != start || !*fmt.str)
    return fmt;

  fmt.str++; // skip the first '%'
  spec->field_width = -1;

  static const struct format_state {
    unsigned char state;
    unsigned char size;
    unsigned char flags_or_double_size;
    unsigned char base;
  } lookup_state[256] = {

      // length
      ['l'] = {0, sizeof(long), sizeof(long long)},
      ['L'] = {0, sizeof(long long)},
      ['h'] = {0, sizeof(short), sizeof(char)},
      ['H'] = {0, sizeof(char)}, // Questionable historical
      ['z'] = {0, sizeof(size_t)},
      ['t'] = {0, sizeof(ptrdiff_t)},

      // non-numerics
      ['c'] = {FORMAT_STATE_CHAR},
      ['s'] = {FORMAT_STATE_STR},
      ['%'] = {FORMAT_STATE_PERCENT_CHAR},

      // numerics
      ['o'] = {FORMAT_STATE_NUM, 0, 0, 8},
      ['x'] = {FORMAT_STATE_NUM, 0, SMALL, 16},
      ['X'] = {FORMAT_STATE_NUM, 0, 0, 16},
      ['d'] = {FORMAT_STATE_NUM, 0, SIGN, 10},
      ['i'] = {FORMAT_STATE_NUM, 0, SIGN, 10},
      ['u'] = {FORMAT_STATE_NUM, 0, 0, 10}};

  const struct format_state *p = lookup_state + (uint8_t)*fmt.str;
  if (p->size) {
    fmt.size = p->size;
  }

  if (p->state) {
    if (p->base)
      spec->base = p->base;
    spec->flags |= p->flags_or_double_size;
    fmt.state = p->state;
    fmt.str++;
    return fmt;
  }

  fmt.state = FORMAT_STATE_INVALID;
  return fmt;
}

// int printf(const char *fmt, ...) {
//   // conversion specification:
//   //  %[$][flags][width][.precision][length modifier]conversion
//   // comment the entire function to use printf in glibc.
//   va_list ap;
//   va_start(ap, fmt);
//   char print_buf[1024];
//   vsnprintf(print_buf, 1024, fmt, ap);
//   va_end(ap);
//   for (int i = 0; print_buf[i]; i++) {
//     putch(print_buf[i]);
//   }
//   return 0;
// }

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
 * @return char* return the terminating null byte.
 */
static char *number(char *str, char *end, uint64_t num,
                    struct printf_spec spec) {
  char sign = '+';
  char tmp[3 * sizeof(num)];
  int  i = 0;
  char digits[] = "0123456789ABCDEF";

  if (spec.base == 16 && spec.flags & SMALL) {
    for (int j = 10; j < sizeof(digits); j++) {
      digits[j] = digits[j] + 32;
    }
  }

  if (spec.flags & SIGN) {
    if ((int64_t)num < 0) {
      num = -(int64_t)num;
      sign = '-';
    }
  }

  do {
    tmp[i++] = digits[num % spec.base];
    num /= spec.base;
  } while (num);

  if (sign == '-' || spec.flags & PLUS) {
    tmp[i++] = sign;
  }
  while (i > 0) {
    if (str < end) {
      *str++ = tmp[--i];
    } else {
      break;
    }
  }
  if (str < end) {
    *str = '\0';
  } else {
    end[-1] = '\0';
  }
  return str + i;
}

// for more reference:
// https://github.com/torvalds/linux/blob/76544811c850a1f4c055aa182b513b7a843868ea/lib/vsprintf.c#L2760
int vsnprintf(char *buf, size_t size, const char *fmt_str, va_list ap) {
  char *str, *end;
  str = buf;
  end = buf + size;

  struct fmt         fmt = {.str = fmt_str, .state = FORMAT_STATE_NONE};
  struct printf_spec spec = {0};

  /* make sure buf is always smaller than end */
  if (end < buf) {
    end = ((void *)-1);
    size = end - buf;
  }

  while (*fmt.str) {
    const char *old_fmt = fmt.str;

    fmt = format_decode(fmt, &spec);
    // printf("state = %d, str = %s, buf = %s, buf + 4 = %s\n", fmt.state,
    // fmt.str, buf, buf + 4);
    switch (fmt.state) {
    case FORMAT_STATE_NONE: {
      int read = fmt.str - old_fmt;
      if (str < end) {
        int copy = read;
        if (copy > end - str) {
          copy = end - str;
        }
        memcpy(str, old_fmt, copy);
      }
      str += read;
      continue;
    }

    case FORMAT_STATE_NUM: {
      uint64_t num;
      if (fmt.size <= sizeof(int32_t))
        num = va_arg(ap, int32_t);
      else
        num = va_arg(ap, int64_t);
      str = number(str, end, num, spec);
      continue;
    }

    case FORMAT_STATE_STR: {
      char *s = va_arg(ap, char *);
      int   read = strlen(s);
      if (str < end) {
        int copy = read;
        if (copy > end - str) {
          copy = end - str;
        }
        memcpy(str, s, copy);
      }
      str += read;
      continue;
    }
    }

    // if (*fmt != '%') {
    //   if (str < end)
    //     *str++ = *fmt;
    //   fmt++;
    //   continue;
    // }
    // fmt++;

    // switch (*fmt) {
    // case 'd': {
    //   int32_t num = va_arg(ap, int32_t);
    //   str = number(str, end - 1, num);
    //   break;
    // }
    // case 's': {
    //   char *s = va_arg(ap, char *);
    //   str = stpcpy(str, s);
    //   break;
    // }
    // }

    // fmt++;
  }
  if (size > 0) {
    if (str < end) {
      *str = '\0';
    } else {
      *(end - 1) = '\0';
    }
  }
  return str - buf;
}

#endif