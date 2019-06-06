CFLAGS=-Wall -std=c11 -g
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)
UNAME=$(shell uname)

ifeq ($(shell uname),Linux)
all: 9ninecc

9ninecc: $(OBJS)
	$(CC) -o 9ninecc $(OBJS) $(LDFLAGS)

$(OBJS): 9ninecc.h

test: 9ninecc
	(cd test_source; make test)
else
all:
	docker-compose run 9ninecc-env make

test:
	docker-compose run 9ninecc-env make test
endif

clean:
	rm -f 9ninecc *.o *~ tmp*
