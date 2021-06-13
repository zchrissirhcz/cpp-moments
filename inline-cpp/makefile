default_target: out

CFLAGS = -O0

small:
	clang++ ${CFLAGS} small.cpp -c -o small.o

big:
	clang++ ${CFLAGS} big.cpp -c -o big.o

main:
	clang++ ${CFLAGS} main.cpp -c -o main.o

out: small big main
	clang++ ${CFLAGS} small.o big.o main.o -o out

.PHONY: clean
clean:
	rm *.o
