#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int printf(const char *fmt, ...) {
  char buffer[1024]; 
  va_list ap;
  va_start(ap, fmt);
  int result = vsprintf(buffer, fmt, ap); 
  va_end(ap);

  for (char *p = buffer; *p != '\0'; p++) {
    putch(*p); 
  }
  return result;
}

//将整数转换为字符串
static void itoa(int n, char *str) {
  char temp[20];
  int i = 0, j = 0;
  if (n == 0) {
    str[i++] = '0';
  } else {
    if (n < 0) {
      str[i++] = '-';
      n = -n;
    }
    while (n > 0) {
      temp[j++] = (n % 10) + '0';
      n /= 10;
    }
    while (j > 0) {
      str[i++] = temp[--j];
    }
  }
  str[i] = '\0';
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  const char *p = fmt;
  char *s;
  int d;
  char buffer[20];
  char *out_ptr = out;

  while (*p) {
    if (*p == '%') {
      p++;
      switch (*p) {
        case 's':
          s = va_arg(ap, char *);
          while (*s) {
            *out_ptr++ = *s++;
          }
          break;
        case 'd':
          d = va_arg(ap, int);
          itoa(d, buffer);
          s = buffer;
          while (*s) {
            *out_ptr++ = *s++;
          }
          break;
        default:
          *out_ptr++ = '%';
          *out_ptr++ = *p;
          break;
      }
    } else {
      *out_ptr++ = *p;
    }
    p++;
  }
  *out_ptr = '\0';
  return out_ptr - out;
}

int sprintf(char *out, const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  int result = vsprintf(out, fmt, ap);
  va_end(ap);
  return result;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  panic("Not implemented");
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  panic("Not implemented");
}

#endif
