#!/bin/bash

try() {
    expected="$1"
    input="$2"
    shift 2

    ./9ninecc "int main(){$input}" > tmp.s
    gcc -o tmp tmp.s "$@"
    ./tmp
    actual="$?"

    if [ "$actual" = "$expected" ]; then
        echo "$input => $actual"
    else
        echo "$expected expected, but got $actual"
        exit 1
    fi
}

try_output() {
    expected="$1"
    input="$2"
    shift 2

    ./9ninecc "int main(){$input}" > tmp.s
    gcc -o tmp tmp.s "$@"
    actual=$(./tmp)

    if [ "$actual" = "$expected" ]; then
        echo "$input => $actual"
    else
        echo "$expected expected, but got $actual"
        exit 1
    fi
}

try_output_raw() {
    expected="$1"
    input="$2"
    shift 2

    ./9ninecc "$input" > tmp.s
    gcc -o tmp tmp.s "$@"
    actual=$(./tmp)

    if [ "$actual" = "$expected" ]; then
        echo "$input => $actual"
    else
        echo "$expected expected, but got $actual"
        exit 1
    fi
}

try 0 "0;"
try 42 "42;"
try 21 "5+20-4;"
try 41 " 12 + 34 - 5 ;"
try 47 "5+6*7;"
try 6 "1 + 10 / 2;"
try 15 "5*(9-6);"
try 4 "(3+5)/2;"
try 5 "-10+15;"
try 10 "-2*-(2+3);"
try 1 "1==1;"
try 0 "1==0;"
try 0 "3*2!=6;"
try 1 "1 + 10 / 2 != 5;"
try 1 "2>1;"
try 0 "2>2;"
try 0 "2>3;"
try 1 "2>=1;"
try 1 "2>=2;"
try 0 "2>=3;"
try 0 "2<1;"
try 0 "2<2;"
try 1 "2<3;"
try 0 "2<=1;"
try 1 "2<=2;"
try 1 "2<=3;"
try 4 "int a;a=2;a*2;"
try 3 "int a;int z;a=1;z=2;a+z;"
try 2 "return 2;3;"
# try 1 "-9223372036854775808 == -9223372036854775807 - 1;" # 最小の負の整数
try 3 "int abc; int abd; abc=1;abd=2;abc+abd;"
try 2 "if(1)return 2; return 3;"
try 3 "if(0)return 2; return 3;"
try 2 "if(1)return 2;else return 3;return 4;"
try 3 "if(0)return 2;else return 3;return 4;"
try 5 "int a; a=0;while(a != 5) a=a+1; return a;"
try 0 "int a; a=0;while(a != 9999999) a=a+1; return 0;"
try 50 "int a; int b; b=0;for(a=0;a<5;a=a+1) b=b+10; return b;"
try 0 "int a;for(a=0; a != 9999999; a=a+1) return 0;"
try 3 "{int a;a=1; a=a+2; return a;}"
try_output OK "foo();" test_source/test1.c
try 45 "return 1+foo()+2;" test_source/test2.c
try_output 1-2-3-4-5-6-7-8 "foo(1,2,3,4,5,6,7,8);" test_source/test3.c
try_output_raw "5" "int main(){pr_int(add(2,3));} int add(int a,int b){return a+b;}" test_source/print.c
try_output_raw "6765" "int main(){pr_int(fib(20));} int fib(int n){if(n <= 2)return 1;else return fib(n-1)+fib(n-2);}" test_source/print.c
try_output_raw 3 'int main() { int x; x = 3; int *y; pr_int(3);}' test_source/print.c
try_output_raw 3 'int main() { int x; x = 3; int *y; y = &x; int z; z = *y; pr_int(z);}' test_source/print.c
try_output_raw 3 'int main() { int x; x = 3; int *y; y = &x; pr_int(*y); }' test_source/print.c
try_output_raw 10 'int main() { int *p; p = getdata(); pr_int(*p); }' test_source/test4.c test_source/print.c
try_output_raw 30 'int main() { int *p; p = getdata(); pr_int(*(p + 2)); }' test_source/test4.c test_source/print.c
try_output_raw 40 'int main() { int **p; p = getdata2(); pr_int(**p); }' test_source/test4.c test_source/print.c
try_output_raw 20 'int main() { int **p; p = getdata2(); pr_int(**(p + 2)); }' test_source/test4.c test_source/print.c
try_output_raw 30 'int main() { int *p; p = getdata(); p = p + 2; pr_int(*p); }' test_source/test4.c test_source/print.c
try_output_raw 20 'int main() { int *p; p = getdata(); p = p + 2; pr_int(*(p - 1)); }' test_source/test4.c test_source/print.c
try_output_raw 20 'int main() { int **p; p = getdata2(); p = p + 2; pr_int(**p); }' test_source/test4.c test_source/print.c
try_output_raw 30 'int main() { int **p; p = getdata2(); p = p + 2; pr_int(**(p - 1)); }' test_source/test4.c test_source/print.c
try_output_raw 2 'int main() { int *p; int *q; p=at(1); q=at(3); pr_int(q-p); }' test_source/test4.c test_source/print.c
try_output_raw -2 'int main() { int *p; int *q; p=at(1); q=at(3); pr_int(p-q); }' test_source/test4.c test_source/print.c
try 4 "return sizeof(5);"
try 4 "int a; return sizeof(a);"
try 8 "int a; return sizeof(&a);"
try 8 "int *a; return sizeof(a);"
try 4 "int *a; return sizeof(*a);"
try 0 "int a[10]; return 0;"
try_output_raw 10 'int main() { int *p; p = getdata(); pr_int(p[0]); }' test_source/test4.c test_source/print.c
try_output_raw 30 'int main() { int *p; p = getdata(); pr_int(p[2]); }' test_source/test4.c test_source/print.c
try_output_raw 10 'int main() { int *p; p = getdata(); pr_int(0[p]); }' test_source/test4.c test_source/print.c
try_output_raw 30 'int main() { int *p; p = getdata(); pr_int(2[p]); }' test_source/test4.c test_source/print.c
try_output_raw 40 'int main() { int **p; p = getdata2(); pr_int(*p[0]); }' test_source/test4.c test_source/print.c
try_output_raw 20 'int main() { int **p; p = getdata2(); pr_int(*p[2]); }' test_source/test4.c test_source/print.c
try_output_raw 40 'int main() { int **p; p = getdata2(); pr_int(*0[p]); }' test_source/test4.c test_source/print.c
try_output_raw 20 'int main() { int **p; p = getdata2(); pr_int(*2[p]); }' test_source/test4.c test_source/print.c
try 1 'int a; int *p; p = &a; *p = 1; return a;'
try_output_raw 1 'int main() { int a[10]; a[0] = 1; pr_int(a[0]);}'  test_source/print.c
try_output_raw 1 'int main() { int a[10]; int *p; p = a; *p = 1; pr_int(a[0]);}'  test_source/print.c
try_output_raw 1 'int main() { int a[10]; int *p; a[5] = 1; p = &a[5]; pr_int(*p);}'  test_source/print.c
try_output_raw 5 'int main() { int a[10]; pr_int(&a[5] - a);}'  test_source/print.c
try_output_raw 10 'int a;int f(int x){a=x;} int main() {f(10);pr_int(a);}'  test_source/print.c
try_output_raw 100 'int g() {return 100;} int a;int f(int x){a=x;} int main() {f(g());pr_int(a);}'  test_source/print.c
try_output_raw 10 'int a[10];int f(int x){a[3]=x;} int main() {f(10);pr_int(a[3]);}'  test_source/print.c
try 3 'char x[3]; x[0]=-1; x[1]=2; int y; y=4; return x[0]+y;'


echo OK

