CC = gcc
FLAGS = -std=c11 -Wall -Wvla -pedantic -g
SRC = src/*.c
HEADERS = headers/*.h
MAIN = main.c
EXECUTABLE = expressionTree

$(EXECUTABLE): $(SRC) $(MAIN)  $(HEADERS)
	$(CC) -o $(EXECUTABLE) $(SRC) $(MAIN) $(FLAGS)

test: $(EXECUTABLE) $(SRC) $(MAIN)
	./$(EXECUTABLE)

valgrind: $(EXECUTABLE) $(SRC) $(MAIN) 
	valgrind ./$(EXECUTABLE)

clean:
	rm -f $(EXECUTABLE)

.PHONY: test clean
