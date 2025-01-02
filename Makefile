CC = gcc
FLAGS = -std=c11 -Wall -Wvla -pedantic -g
SRC = *.c
HEADERS = *.h
EXECUTABLE = expressionTree

$(EXECUTABLE): $(SRC) $(HEADERS)
	$(CC) -o $(EXECUTABLE) $(SRC) $(FLAGS)

test: $(EXECUTABLE) $(SRC)
	./$(EXECUTABLE)

valgrind: $(SRC)
	valgrind ./$(EXECUTABLE)

clean:
	rm -f $(EXECUTABLE)

.PHONY: test clean
