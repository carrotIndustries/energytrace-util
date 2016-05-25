TARGET=et
SRC = $(TARGET).c

CFLAGS = -I/usr/include/libmsp430 -lmsp430

all: $(TARGET)
$(TARGET): $(SRC) 
	gcc $(CFLAGS) -o $@ $<
