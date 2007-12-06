CFLAGS=-Wall -O2

all: base85

base85: base85.c main.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f base85
