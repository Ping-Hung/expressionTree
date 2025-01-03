# expressionTree
This is an attempt to create a simple scanner (tokenizer) and parser that builds a parse tree from a user specified mathematical expression.

# Compile/Build Instructions
Assume `gcc` and `Make` are available on the machine.

-  refer to ``Makefile`` for build, test, and clean instructions.
- if built successfully, an executable named ``expressionTree`` will be present in the working directory.
- if ``valgrind`` is installed on the machine, ``make valgrind`` will run the executable with valgrind to check for memory errors.