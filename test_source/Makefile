SRCS=$(filter-out test.c test_main.c,$(wildcard *.c)) test_main.c
OBJS=$(SRCS:.c=.o)
CC=gcc
CFLAGS=-g

tmp: $(OBJS) test.o
	$(CC) -o tmp $(OBJS) test.o $(LDFLAGS)

$(OBJS): try.h

test_main.c: test.c gen_test_main.pl
	perl gen_test_main.pl test.c |clang-format > test_main.c

test.s: ../9ninecc test.c
	../9ninecc test.c > test.s

test.o: test.s
	$(CC) -c test.s

test: tmp
	./tmp

clean:
	rm -f *~ tmp* test_main.c test.s
