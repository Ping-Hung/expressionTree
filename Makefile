CC = gcc
FLAGS = -std=c11 -Wall -Wvla -pedantic -g
SRC = *.c
HEADERS = *.h
EXECUTABLE = expressionTree

$(EXECUTABLE): $(SRC)
	$(CC) -o $(EXECUTABLE) $(SRC) $(FLAGS)

test: $(EXECUTABLE)
	./$(EXECUTABLE)

clean:
	rm -f $(EXECUTABLE)

.PHONY: test clean
