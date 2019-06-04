CFLAGS=-Wall -std=c11 -g
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

9ninecc: $(OBJS)
	$(CC) -o 9ninecc $(OBJS) $(LDFLAGS)

$(OBJS): 9ninecc.h

test: 9ninecc
	(cd test_source; make test)

clean:
	rm -f 9ninecc *.o *~ tmp*
