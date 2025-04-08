/*
 * stdio.c
 *
 * @project: ysyx
 * @author: Juntong Chen (dev@jtchen.io)
 * @created: 2025-03-28 11:37:29
 * @modified: 2025-03-29 01:49:24
 *
 * Copyright (c) 2025 Juntong Chen. All rights reserved.
 */

#include "klibtest.h"
#include <limits.h>

static const char *data[] = {
    "Hello, ", "World!", "Hello, World!", "1234567891011", "Hello, 1234",
};
static const int nums[] = {1234,          5678,         91011,   0,
                           INT_MAX / 17,  INT_MAX,      INT_MIN, INT_MIN + 1,
                           UINT_MAX / 17, INT_MAX / 17, UINT_MAX};

static const char *golden[] = {
    "126322567|2147483647|-2147483648|-2147483647|252645135|126322567|-1"};

void test_sprintf() {
  START("sprintf()");
  char buf[128];
  int  n = sprintf(buf, "Hello, %s", data[1]);
  assert(n == strlen(data[0]) + strlen(data[1]));
  assert(strcmp(buf, data[2]) == 0);
  n = sprintf(buf, "%s%d", data[0], nums[0]);
  assert(n == strlen(data[0]) + strlen("1234"));
  assert(strcmp(buf, data[4]) == 0);
  n = sprintf(buf, data[2]);
  assert(n == strlen(data[2]));
  assert(strcmp(buf, data[2]) == 0);
  n = sprintf(buf, "%s", data[3]);
  assert(n == strlen(data[3]));
  assert(strcmp(buf, data[3]) == 0);
  PASS("sprintf()");
}

void test_snprintf() {
  START("snprintf()");
  char buf[128];
  int  n = snprintf(buf, sizeof(buf), "%s%s", data[0], data[1]);
  assert(n == strlen(data[0]) + strlen(data[1]));
  assert(strcmp(buf, data[2]) == 0);
  n = snprintf(buf, sizeof(buf), "%s%d", data[0], nums[0]);
  assert(n == strlen(data[0]) + strlen("1234"));
  assert(strcmp(buf, data[4]) == 0);
  n = snprintf(buf, sizeof(buf), data[2]);
  assert(n == strlen(data[2]));
  assert(strcmp(buf, data[2]) == 0);
  n = snprintf(buf, 4, "%s", data[0]);
  assert(n == 7);
  assert(strcmp(buf, "Hel") == 0);
  n = snprintf(buf, 0, "%s%d", data[1], nums[1]);
  assert(n == strlen(data[1]) + 4);
  assert(strcmp(buf, "Hel") == 0);
  n = snprintf(buf, 3, "%d", nums[1]);
  assert(n == 4);
  assert(strcmp(buf, "56") == 0);

  n = snprintf(buf, sizeof(buf), "0x%x", 0x1234aaee);
  assert(n == 10);
  assert(strcmp(buf, "0x1234aaee") == 0);

  n = snprintf(buf, sizeof(buf), "0x%X", 0x1234aaee);
  assert(n == 10);
  assert(strcmp(buf, "0x1234AAEE") == 0);

  n = snprintf(buf, sizeof(buf), "0o%o", 01234567);
  assert(n == 9);
  assert(strcmp(buf, "0o1234567") == 0);

  n = snprintf(buf, sizeof(buf), "%d|%d|%d|%d|%d|%d|%d", nums[4], nums[5],
               nums[6], nums[7], nums[8], nums[9], nums[10]);
  // printf("n = %d, [%s]\n", n, buf);
  assert(n = 67);
  assert(strcmp(buf, golden[0]) == 0);

  PASS("snprintf()");
}

void test_io() {
  test_sprintf();
  test_snprintf();
}