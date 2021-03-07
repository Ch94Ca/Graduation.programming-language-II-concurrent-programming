src = $(wildcard src/*.c)
obj = $(src:.c=.o)

CFLAGS = -Wall -lpthread -g -O0

exe: $(obj)
	$(CC) -g -o $@ $^ $(CFLAGS)
clean:
	rm -rf *o exe
	rm -rf src/*.o
