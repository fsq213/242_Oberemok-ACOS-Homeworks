CC = gcc
CFLAGS = -Wall -pthread
TARGET = array_sum
SOURCES = array_sum.c

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) -o $(TARGET) $(SOURCES)

clean:
	rm -f $(TARGET)

run: $(TARGET)
	./$(TARGET)