# ExpressionTree
This is an attempt to create a simple scanner (tokenizer) and parser that builds a parse tree from a user specified mathematical expression.

# Lexical Symbols:
The tokenizer recognizes the following as valid symbols: 
- variables (regex `[A-Za-z_]+[A-Za-z0-9_]$`) 
- integral numeric literals (regex `^[0-9]+$)
- operators (one of `{'+', '-', '*', '/', '%', '++', '--'}`)

# Grammar:
```
			expr	:=  expr '+' Mult
				|   expr '-' Mult
				|   Mult

			Mult	:= Mult '*' Unary
				|  Mult '/' Unary
				|  Mult '%' Unary
				|  Mult Atom
				|  Unary

			Unary	:= '+' Unary
				|  '-' Unary
				|  IncDec

			IncDec  := '++' IncDec
				|  '--' IncDec
				|  IncDec '++'
				|  IncDec '--'
				|  Atom

			Atom	:= TOK_LIT | TOK_VAR | '(' expr ')'

			TOK_VAR	:= ^[A-Za-z]+[_A-Za-z0-9]*$
			TOK_LIT	:= ^[0-9]+$
```

- **Note**: implicit multiplication is supported via the syntax 
``` 
	(TOK_LIT|TOK_VAR) <space> (TOK_LIT|TOK_VAR) 
```
This differs from regular programming language grammar for which above syntax could mean `(TOK_LIT|TOK_VAR)` initialization.

# Compile/Build Instructions
- If an `ExpressionTree` is built successfully, it will be printed onto a file named `parseTree.txt` in this directory.
  - `parseTree.txt` will display the pre-order printing of the `ExpressionTree`, indentations will mark the "depth" of each node 
  (2 nodes with same indentations meant they are on the same level (and are siblings)).


Assume `gcc` and `Make` are available on the machine.

- refer to `Makefile` for build, test, and clean instructions.
- On successful builds, an executable named `expressionTree` will be present in the working directory.
- if `valgrind` is installed on the machine, `make valgrind` will run the executable with valgrind to check for memory errors.

## Rules Summary

### Simple Build

```
	make
```

### Build Then Execute

```
	make test
```

### Build, Test, Execute, then Check Memory Fault

- assume `valgrind` is installed on machine

```	
	make valgrind
```

### Remove Executable

```
	make clean
```


