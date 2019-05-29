9ninecc: 9ninecc.c

test: 9ninecc
	./test.sh

clean:
	rm -f 9ninecc *.o *~ tmp*
