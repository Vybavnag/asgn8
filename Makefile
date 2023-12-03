CC = clang
CFLAGS = -Werror -Wall -Wextra -Wconversion -Wdouble-promotion -Wstrict-prototypes -pedantic
LFLAGS = -lm

EXECS = huff dehuff bwtest brtest nodetest pqtest

all: $(EXECS)

huff: huff.o bitwriter.o bitreader.o node.o pq.o
	$(CC) huff.o bitwriter.o bitreader.o node.o pq.o $(LFLAGS) -o huff

dehuff: dehuff.o bitwriter.o bitreader.o node.o pq.o
	$(CC) dehuff.o bitwriter.o bitreader.o node.o pq.o $(LFLAGS) -o dehuff

bwtest: bwtest.o bitwriter.o
	$(CC) bwtest.o bitwriter.o $(LFLAGS) -o bwtest

brtest: brtest.o bitreader.o
	$(CC) brtest.o bitreader.o $(LFLAGS) -o brtest

nodetest: nodetest.o node.o
	$(CC) nodetest.o node.o $(LFLAGS) -o nodetest

pqtest: pqtest.o pq.o node.o
	$(CC) pqtest.o pq.o node.o $(LFLAGS) -o pqtest

huff.o: huff.c
	$(CC) $(CFLAGS) -c huff.c -o huff.o

dehuff.o: dehuff.c
	$(CC) $(CFLAGS) -c dehuff.c -o dehuff.o

bwtest.o: bwtest.c
	$(CC) $(CFLAGS) -c bwtest.c -o bwtest.o

brtest.o: brtest.c
	$(CC) $(CFLAGS) -c brtest.c -o brtest.o

nodetest.o: nodetest.c
	$(CC) $(CFLAGS) -c nodetest.c -o nodetest.o

pqtest.o: pqtest.c
	$(CC) $(CFLAGS) -c pqtest.c -o pqtest.o

bitwriter.o: bitwriter.c
	$(CC) $(CFLAGS) -c bitwriter.c -o bitwriter.o

bitreader.o: bitreader.c
	$(CC) $(CFLAGS) -c bitreader.c -o bitreader.o

node.o: node.c
	$(CC) $(CFLAGS) -c node.c -o node.o

pq.o: pq.c
	$(CC) $(CFLAGS) -c pq.c -o pq.o

clean:
	rm -rf $(EXECS) *.o

format:
	clang-format -i -style=file *.[ch]

.PHONY: all huff dehuff bwtest brtest nodetest pqtest clean format
