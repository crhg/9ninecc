# ダミーのcmakeファイル
# 9nineccはlinuxのABI前提なのでmacでは動かないはず

cmake_minimum_required(VERSION 2.6)

project(9ninecc-test)

add_executable(dummy test.c try.c test1.c test2.c test3.c test4.c print.c dummy_main.c)
