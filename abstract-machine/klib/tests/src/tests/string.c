/*
 * string.c
 *
 * @project: ysyx
 * @author: Juntong Chen (dev@jtchen.io)
 * @created: 2025-03-28 00:34:48
 * @modified: 2025-03-28 12:28:15
 *
 * Copyright (c) 2025 Juntong Chen. All rights reserved.
 */
#include "klibtest.h"
#define N 32

static uint8_t data[N];
static uint8_t *ret;
static char    str1[N];
static char    str2[N];
static char    str3[N];

void reset_data() {
  int i;
  for (i = 0; i < N; i++) {
    data[i] = i + 1;
  }
}

void reset_str() {
  for (int i = 0; i < N - 1; i++) {
    str1[i] = 'A' + i;
    str2[i] = 'a' + i;
    str3[i] = '1' + i;
  }
  str1[N - 1] = '\0';
  str2[N - 1] = '\0';
}

// check if s1 and s2 are equal
void check_str_eq(char *s1, char *s2) {
  int i;
  for (i = 0; i < N; i++) {
    assert(s1[i] == s2[i]);
    if (s1[i] == '\0' && s2[i] == '\0') {
      break;
    }
  }
}

void check_str_seq(char *s1, char val) {
  int i;
  for (i = 0; i < N; i++) {
    assert(s1[i] == val + i);
    if (s1[i] == '\0') {
      break;
    }
  }
}

// check if data[l, r) is a sequence starting from val
void check_data_seq(int l, int r, int val) {
  int i;
  for (i = l; i < r; i++) {
    assert(data[i] == val + i - l);
  }
}

// check if data[l, r) is all equal to val
void check_data_eq(int l, int r, int val) {
  int i;
  for (i = l; i < r; i++) {
    assert(data[i] == val);
  }
}

void test_memw() {
  START("memset()");
  int l, r;
  for (l = 0; l < N; l++) {
    for (r = l + 1; r <= N; r++) {
      reset_data();
      uint8_t val = (l + r) / 2;
      memset(data + l, val, r - l);
      check_data_seq(0, l, 1);
      check_data_eq(l, r, val);
      check_data_seq(r, N, r + 1);
    }
  }
  PASS("memset()");

  START("memcpy()");
  for (l = 0; l < N / 2; l++) {
    for (r = l + 1; r <= N; r++) {
      reset_data();
      uint8_t val = data[l];
      memcpy(data + r, data + l, r - l);
      check_data_seq(0, l, 1);
      check_data_seq(r, r + (r - l), val);
    }
  }
  PASS("memcpy()");

  START("memmove()");
  for (l = 0; l < N; l++) {
    for (r = l + 1; r <= N; r++) {
      reset_data();
      int newl = N - (r - l);
      ret = memmove(data + newl, data + l, r - l);
      check_data_seq(newl, N, l + 1);
      assert(ret == data + newl);
    }
    reset_data();
    ret = memmove(data + l, data + l, N - l);
    check_data_seq(l, N, l + 1);
    assert(ret == data + l);
  }
  PASS("memmove()");
}

void test_memr() {}

void test_strw() {
  START("str(n)cpy()");
  for (int i = 0; i < N; i++) {
    reset_str();
    strcpy(str2, str1 + i);
    check_str_eq(str1 + i, str2);
    for (int j = N - i - 1; j >= 0; j--) {
      reset_str();
      strncpy(str2, str1 + i, j);
      str2[j] = '\0'; // Add null terminator after strncpy
      strcpy(str3, str1 + i);
      str3[j] = '\0'; // This should be j not j+1
      check_str_eq(str2, str3);
      assert(strlen(str2) == j);
    }
  }
  PASS("strcpy()");
  PASS("strncpy()");
  PASS("strlen()");

  START("strcat()");
  char a[32] = "Hello, ";
  char b[32] = "World!";
  char c[32] = "Hello, World!";
  char d[32] = "";
  strcat(a, b);
  check_str_eq(a, c);
  strcat(d, c);
  check_str_eq(d, c);
  PASS("strcat()");
}

void test_strr() {
  START("str(n)cmp()");
  for (int i = 0; i < N - 1; i++) {
    reset_str();
    assert(strcmp(str1, str2) < 0);
    assert(strcmp(str1 + i, str2 + i) < 0);
    assert(strcmp(str1 + i, str1 + i) == 0);
    assert(strcmp(str2 + i, str2 + i) == 0);
    assert(strcmp(str1 + i, str2) < 0);
    assert(strcmp(str2, str1 + i) > 0);
    assert(strlen(str1 + i) == N - 1 - i);
    for (int j = 0; j < i; j++) {
      assert(strncmp(str1 + j, str2 + j, i - j) < 0);
      assert(strncmp(str1 + j, str1 + j, i - j) == 0);
      assert(strncmp(str2 + j, str2 + j, i - j) == 0);
      assert(strncmp(str1 + j, str2 + j, i - j) < 0);
      assert(strncmp(str2 + j, str1 + j, i - j) > 0);
    }
  }
  PASS("strcmp()");
  PASS("strncmp()");
}