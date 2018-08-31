TARGET=energytrace
SRC = $(TARGET).c

CFLAGS = -I/usr/include/libmsp430 -lmsp430

all: $(TARGET)
$(TARGET): $(SRC) 
	gcc $(CFLAGS) -o $@ $<
clean:
	rm -f $(TARGET)

run: all
	./energytrace 5

install: all
	install -t /usr/local/bin $(TARGET)


