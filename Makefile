CC = gcc
CFLAGS = -std=c89 -Wpedantic

SOURCES = src/main.c

EXECUTABLE = out/myprogram.exe

all: $(EXECUTABLE)

$(EXECUTABLE): $(SOURCES)
	$(CC) $(CFLAGS) -o $@ $^

run : $(EXECUTABLE)
	./$(EXECUTABLE)

clean:
	rm -f out/*.exe