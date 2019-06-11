// @try_ret test0 0
int test0() { 0; }
// @end

// @try_ret test1 42
int test1() { 42; }
// @end

// @try_ret test2 21
int test2() { 5 + 20 - 4; }
// @end

// @try_ret test3 41
int test3() { 12 + 34 - 5; }
// @end

// @try_ret test4 47
int test4() { 5 + 6 * 7; }
// @end

// @try_ret test5 6
int test5() { 1 + 10 / 2; }
// @end

// @try_ret test6 15
int test6() { 5 * (9 - 6); }
// @end

// @try_ret test7 4
int test7() { (3 + 5) / 2; }
// @end

// @try_ret test8 5
int test8() { -10 + 15; }
// @end

// @try_ret test9 10
int test9() { -2 * -(2 + 3); }
// @end

// @try_ret test10 1
int test10() { 1 == 1; }
// @end

// @try_ret test11 0
int test11() { 1 == 0; }
// @end

// @try_ret test12 0
int test12() { 3 * 2 != 6; }
// @end

// @try_ret test13 1
int test13() { 1 + 10 / 2 != 5; }
// @end

// @try_ret test14 1
int test14() { 2 > 1; }
// @end

// @try_ret test15 0
int test15() { 2 > 2; }
// @end

// @try_ret test16 0
int test16() { 2 > 3; }
// @end

// @try_ret test17 1
int test17() { 2 >= 1; }
// @end

// @try_ret test18 1
int test18() { 2 >= 2; }
// @end

// @try_ret test19 0
int test19() { 2 >= 3; }
// @end

// @try_ret test20 0
int test20() { 2 < 1; }
// @end

// @try_ret test21 0
int test21() { 2 < 2; }
// @end

// @try_ret test22 1
int test22() { 2 < 3; }
// @end

// @try_ret test23 0
int test23() { 2 <= 1; }
// @end

// @try_ret test24 1
int test24() { 2 <= 2; }
// @end

// @try_ret test25 1
int test25() { 2 <= 3; }
// @end

// @try_ret test26 4
int test26() {
  int a;
  a = 2;
  a * 2;
}
// @end

// @try_ret test27 3
int test27() {
  int a;
  int z;
  a = 1;
  z = 2;
  a + z;
}
// @end

// @try_ret test28 2
int test28() {
  return 2;
  3;
}
// @end

// @try_ret test29 3
int test29() {
  int abc;
  int abd;
  abc = 1;
  abd = 2;
  abc + abd;
}
// @end

// @try_ret test30 2
int test30() {
  if (1)
    return 2;
  return 3;
}
// @end

// @try_ret test31 3
int test31() {
  if (0)
    return 2;
  return 3;
}
// @end

// @try_ret test32 2
int test32() {
  if (1)
    return 2;
  else
    return 3;
  return 4;
}
// @end

// @try_ret test33 3
int test33() {
  if (0)
    return 2;
  else
    return 3;
  return 4;
}
// @end

// @try_ret test34 5
int test34() {
  int a;
  a = 0;
  while (a != 5)
    a = a + 1;
  return a;
}
// @end

// @try_ret test35 0
int test35() {
  int a;
  a = 0;
  while (a != 9999999)
    a = a + 1;
  return 0;
}
// @end

// @try_ret test36 50
int test36() {
  int a;
  int b;
  b = 0;
  for (a = 0; a < 5; a = a + 1)
    b = b + 10;
  return b;
}
// @end

// @try_ret test37 0
int test37() {
  int a;
  for (a = 0; a != 9999999; a = a + 1)
    return 0;
}
// @end

// @try_ret test38 3
int test38() {
  {
    int a;
    a = 1;
    a = a + 2;
    return a;
  }
}
// @end

// @try_out test39 OK
int test39() { foo1(); }
// @end

// @try_ret test40 45
int test40() { return 1 + foo2() + 2; }
// @end

// @try_out test41 1-2-3-4-5-6-7-8
int test41() { foo3(1, 2, 3, 4, 5, 6, 7, 8); }
// @end

// @try_out test42 "5"
int test42() { pr_int(add(2, 3)); }
int add(int a, int b) { return a + b; }
// @end

// @try_out test43 "6765"
int test43() { pr_int(fib(20)); }
int fib(int n) {
  if (n <= 2)
    return 1;
  else
    return fib(n - 1) + fib(n - 2);
}
// @end

// @try_out test44 3
int test44() {
  int x;
  x = 3;
  int *y;
  pr_int(3);
}
// @end

// @try_out test45 3
int test45() {
  int x;
  x = 3;
  int *y;
  y = &x;
  int z;
  z = *y;
  pr_int(z);
}
// @end

// @try_out test46 3
int test46() {
  int x;
  x = 3;
  int *y;
  y = &x;
  pr_int(*y);
}
// @end

// @try_out test47 10
int test47() {
  int *p;
  p = getdata();
  pr_int(*p);
}
// @end

// @try_out test48 30
int test48() {
  int *p;
  p = getdata();
  pr_int(*(p + 2));
}
// @end

// @try_out test49 40
int test49() {
  int **p;
  p = getdata2();
  pr_int(**p);
}
// @end

// @try_out test50 20
int test50() {
  int **p;
  p = getdata2();
  pr_int(**(p + 2));
}
// @end

// @try_out test51 30
int test51() {
  int *p;
  p = getdata();
  p = p + 2;
  pr_int(*p);
}
// @end

// @try_out test52 20
int test52() {
  int *p;
  p = getdata();
  p = p + 2;
  pr_int(*(p - 1));
}
// @end

// @try_out test53 20
int test53() {
  int **p;
  p = getdata2();
  p = p + 2;
  pr_int(**p);
}
// @end

// @try_out test54 30
int test54() {
  int **p;
  p = getdata2();
  p = p + 2;
  pr_int(**(p - 1));
}
// @end

// @try_out test55 2
int test55() {
  int *p;
  int *q;
  p = at(1);
  q = at(3);
  pr_int(q - p);
}
// @end

// @try_out test56 -2
int test56() {
  int *p;
  int *q;
  p = at(1);
  q = at(3);
  pr_int(p - q);
}
// @end

// @try_ret test57 4
int test57() { return sizeof(5); }
// @end

// @try_ret test58 4
int test58() {
  int a;
  return sizeof(a);
}
// @end

// @try_ret test59 8
int test59() {
  int a;
  return sizeof(&a);
}
// @end

// @try_ret test60 8
int test60() {
  int *a;
  return sizeof(a);
}
// @end

// @try_ret test61 4
int test61() {
  int *a;
  return sizeof(*a);
}
// @end

// @try_ret test62 0
int test62() {
  int a[10];
  return 0;
}
// @end

// @try_out test63 10
int test63() {
  int *p;
  p = getdata();
  pr_int(p[0]);
}
// @end

// @try_out test64 30
int test64() {
  int *p;
  p = getdata();
  pr_int(p[2]);
}
// @end

// @try_out test65 10
int test65() {
  int *p;
  p = getdata();
  pr_int(0 [p]);
}
// @end

// @try_out test66 30
int test66() {
  int *p;
  p = getdata();
  pr_int(2 [p]);
}
// @end

// @try_out test67 40
int test67() {
  int **p;
  p = getdata2();
  pr_int(*p[0]);
}
// @end

// @try_out test68 20
int test68() {
  int **p;
  p = getdata2();
  pr_int(*p[2]);
}
// @end

// @try_out test69 40
int test69() {
  int **p;
  p = getdata2();
  pr_int(*0 [p]);
}
// @end

// @try_out test70 20
int test70() {
  int **p;
  p = getdata2();
  pr_int(*2 [p]);
}
// @end

// @try_ret test71 1
int test71() {
  int a;
  int *p;
  p = &a;
  *p = 1;
  return a;
}
// @end

// @try_out test72 1
int test72() {
  int a[10];
  a[0] = 1;
  pr_int(a[0]);
}
// @end

// @try_out test73 1
int test73() {
  int a[10];
  int *p;
  p = a;
  *p = 1;
  pr_int(a[0]);
}
// @end

// @try_out test74 1
int test74() {
  int a[10];
  int *p;
  a[5] = 1;
  p = &a[5];
  pr_int(*p);
}
// @end

// @try_out test75 5
int test75() {
  int a[10];
  pr_int(&a[5] - a);
}
// @end

// @try_out test76 10
int a76;
int f76(int x) { a76 = x; }
int test76() {
  f76(10);
  pr_int(a76);
}
// @end

// @try_out test77 100
int g77() { return 100; }
int a77;
int f77(int x) { a77 = x; }
int test77() {
  f77(g77());
  pr_int(a77);
}
// @end

// @try_out test78 10
int a78[10];
int f78(int x) { a78[3] = x; }
int test78() {
  f78(10);
  pr_int(a78[3]);
}
// @end

// @try_ret test79 3
int test79() {
  char x[3];
  x[0] = -1;
  /* x[1] = 2; */
  int y;
  y = 4;
  /* y = x[0] + y; */
  y = y + x[0];
  /* return x[0] + y; */
  return y;
}
// @end

// @try_out test80 hoge
int test80() { try_printf("hoge"); }
// @end

// @try_out test81 hoge
int test81() { /* comment */ try_printf("hoge"); }
// @end

// @try_ret test82 80
int a82[10][20];
int test82() {
    return sizeof(a82[1]);
}
// @end

// @try_ret test83 42
int x83 = 42;
int test83() {
    return x83;
}

// @try_out test84 foo
char *s84 = "foo";
int test84() {
    try_printf("%s", s84);
}

// @try_out test85 oo
char *s85 = "foo" + 1;
int test85() {
    try_printf("%s", s85);
}

// @try_ret test86 86
int x86[3];
int *p86 = x86;
int test86() {
    x86[1] = 86;
    return *(p86 + 1);
}

// @try_ret test87 87
int x87[3];
int *p87 = &x87[1];
int test87() {
    x87[1] = 87;
    return *p87;
}

// @try_ret test88 2
int a88[] = {1,2};
int test88() {
    return a88[1];
}
// @end

// @try_ret test89 89
int test89() {
    int x = 89;
    return x;
}
// @end

// @try_out test90 test90
int test90() {
    char *s = "test90";
    try_printf("%s", s);
}
// @end

// @try_out test91 test91
int test91() {
    char s[7] = "test91";
    try_printf("%s", s);
}
// @end

// @try_ret test92 92
int test92() {
    int x[3] = {91, 92};
    return x[1];
}
// @end

// @try_ret test93 1
int test93() {
    int x[] = {91, 92, 93};
    return sizeof(x) == sizeof(x[0]) *3;
}
// @end

// @try_ret test94 1
int test94() {
    char s[] = "hoge";
    return sizeof(s) == 5;
}
// @end

// @try_ret test95 0
int test95() {
    int x = 5;
    while ((x = x - 1) > 0);
    return x;
}
// @end

// @try_ret test96 8
int test96() {
    struct {
        int x;
        int y;
    } a;
    return sizeof(a);
}
// @end

// @try_ret test97 97
int test97() {
    struct {
        int x;
        int y;
    } a;
    (&a)->x = 90;
    (&a)->y = 7;
    return (&a)->x + (&a)->y;
}
// @end

// @try_ret test98 98
int test98() {
    struct {
        int x;
        int y;
    } a;
    a.x = 90;
    a.y = 8;
    return a.x + a.y;
}
// @end

// @try_ret test99 99
int test99() {
    struct {
        int x;
        int y;
    } a[10];
    int i;
    for (i = 0; i < sizeof(a) / sizeof(a[0]); i = i + 1) {
        a[i].x = i * 10;
        a[i].y = i;
    }
    return a[9].x + a[9].y;
}
// @end

// @try_out test100 "2,1,0,"
int test100() {
    struct a {
        struct a *next;
        int x;
    } a[10];

    a[0].next = 0;
    a[0].x = 0;
    a[1].next = &a[0];
    a[1].x = 1;
    a[2].next = &a[1];
    a[2].x = 2;

    struct a *p;
    for (p = &a[2]; p != 0; p = p->next) {
        try_printf("%d,", p->x);
    }
}
// @end