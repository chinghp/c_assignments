CFLAGS = -I/opt/SDL/2.0.3/include -Wl,-rpath,/opt/SDL/2.0.3/lib -lSDL2 -O4 -Wall -Wextra -Wfloat-equal -pedantic -std=c89 -lm
INCS = neillsdl2.h
TARGET = huffsdl
SOURCES =  neillsdl2.c $(TARGET).c
LIBS =  -L/opt/SDL/2.0.3/lib
CC = gcc


all: $(TARGET)

$(TARGET): $(SOURCES) $(INCS)
	$(CC) $(SOURCES) -o $(TARGET) $(CFLAGS) $(LIBS)

clean:
	rm -f $(TARGET)

run: all
	$(TARGET)
