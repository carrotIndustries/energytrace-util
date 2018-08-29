TARGET=et
SRC = $(TARGET).c

CFLAGS = -I/usr/include/libmsp430 -lmsp430

all: $(TARGET)
$(TARGET): $(SRC) 
	gcc $(CFLAGS) -o $@ $<
clean:
	rm -f et

run: all
	./et 5

install: all
	cp et /usr/local/bin/
