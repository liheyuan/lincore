CC = gcc
CFLAGS = -Wall -O3
COMPILE = $(CC) $(CFLAGS) -c
OBJFILES := $(patsubst %.c,%.o,$(wildcard *.c))

all: lincore

lincore: $(OBJFILES)
	$(CC) -o ./bin/lincore $(OBJFILES)

clean:
	rm -rf ./bin/lincore *.o

%.o: %.c
	$(COMPILE) -o $@ $<

 
