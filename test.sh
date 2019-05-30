#!/bin/bash

try() {
    expected="$1"
    input="$2"
    shift 2

    ./9ninecc "$input" > tmp.s
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
try 4 "a=2;a*2;"
try 3 "a=1;z=2;a+z;"
try 2 "return 2;3;"
# try 1 "-9223372036854775808 == -9223372036854775807 - 1;" # 最小の負の整数
try 3 "abc=1;abd=2;abc+abd;"
try 2 "if(1)return 2; return 3;"
try 3 "if(0)return 2; return 3;"
try 2 "if(1)return 2;else return 3;return 4;"
try 3 "if(0)return 2;else return 3;return 4;"
try 5 "a=0;while(a != 5) a=a+1; return a;"
try 0 "a=0;while(a != 9999999) a=a+1; return 0;"
try 50 "b=0;for(a=0;a<5;a=a+1) b=b+10; return b;"
try 0 "for(a=0; a != 9999999; a=a+1) return 0;"
try 3 "{a=1; a=a+2; return a;}"
try_output OK "foo();" test_source/test1.c
try 45 "return 1+foo()+2;" test_source/test2.c
try_output 1-2-3-4-5-6-7-8 "foo(1,2,3,4,5,6,7,8);" test_source/test3.c

echo OK

