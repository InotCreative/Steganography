CC = gcc
CFLAGS = -Wall -pedantic -g
LIBS = -lpng -lm
TARGET = stegano
SRC = stengography.c

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(LIBS)
	
clean:
	rm -f $(TARGET)