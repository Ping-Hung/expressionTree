CC = gcc
FLAGS = -std=c17 -Wall -Werror -Wvla -pedantic -g
SRC = src/*.c
HEADERS = headers/*.h
MAIN = main.c
EXECUTABLE = expressionTree
VECHO = @echo

$(EXECUTABLE): $(SRC) $(MAIN)  $(HEADERS)
	$(VECHO) "regular build"
	$(CC) -o $(EXECUTABLE) $(SRC) $(MAIN) $(FLAGS)

test: $(EXECUTABLE) $(SRC) $(MAIN)
	./$(EXECUTABLE)

valgrind: $(EXECUTABLE) $(SRC) $(MAIN) 
	valgrind -s --leak-check=full --track-origins=yes ./$(EXECUTABLE)  

clean:
	rm -f $(EXECUTABLE)

.PHONY: test clean valgrind
