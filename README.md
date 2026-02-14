# ExpressionTree
This is an attempt to create a simple scanner (tokenizer) and parser that builds a parse tree from a
user specified mathematical expression, where Pratt parsing is used as the main tree building
algorithm.

# Lexical Symbols
The tokenizer recognizes the following as valid symbols: 
- variables (regex `^[A-Za-z_]+[A-Za-z0-9_]$`) 
- integral numeric literals (regex `^[0-9]+$`)
- operators (one of `{'+', '-', '*', '/', '%', '++', '--'}`) 

Put simply, this section describes "meaningful" symbols to this program using regex

# Grammar:
- The following context free grammar defines how the lexical symbols could be ordered to form meaningful
  mathematical expression.
- Non-terminals are lower-cased words like `expr`, `mult`, `unary`
- Terminals are `TOK_LIT` and `TOK_VAR`, which are defined with regex
- The precedence of each rule will determine how "tall" they are in the resulting AST, in the
  following grammar, the rules/non-terminals that are closer to the bottom have higher precedence.

```
		expr	:=  expr '+' mult
                |   expr '-' mult
                |   mult

		mult	:= mult '*' unary
                |  mult '/' unary
                |  mult '%' unary
                |  mult atom		// implicit multiplication
                |  unary

		unary	:= '+' unary
                |  '-' unary
                |  incdec

		incdec  := '++' incdec
                |  '--' incdec
                |  incdec '++'
                |  incdec '--'
                |  atom

		atom	:= TOK_LIT | TOK_VAR | '(' expr ')'

		TOK_VAR	:= ^[A-Za-z]+[_A-Za-z0-9]*$
		TOK_LIT	:= ^[0-9]+$
```

- **Note**: implicit multiplication is supported via the syntax 
``` 
	(TOK_LIT|TOK_VAR) <space> (TOK_LIT|TOK_VAR) 
```
This differs from regular programming language grammar for which above syntax could mean `(TOK_LIT|TOK_VAR)` initialization.

## Operator Precedence in Above Grammar
```
	("+"|"-") < ("*"|"/"|"%") < ("++"|"--")
```

## Caveats
### Notes on `IncDec` (`++` or `--`)
- They are right associative and have the highest precedence, meaning that expression `a b ++ ⇔  a * (b++)`
  and `++ a b ⇔  (++a) * b`.
- Their respective parse trees are: 
  ```
		"*"
		 |__"a"
		 |__"++"
		  |__"b"
  ```
  and
  ```
		"*"
		 |__"++"
		  |__"a"
		 |__"b"
  ```

### Warning on Mixing `IncDec` and Implicit Multiplication
- Mixing prefix `IncDec`, postfix `IncDec`, and implicit multiplication together is **strongly discouraged**,
  however, this project did handle them.

#### Example 1
The expression ``a ++ b`` obeys the grammar rules defined above, and there are 2 ways the `++`
operator could be associated with variables `a` and `b`.
1. ``a * (++b)``
2. ``(a++) * b``

This project chooses the second way, which says _"any `++` or `--` that is preceeded by either a
variable or literal is going to be treated as unary postfix increment/decrement expressions"._

#### Example 2
Consider the expression ``a++ --b``, there are 2 possible groupings:
1. ``a++ --b ⇔ (a++) * (--b)``
2. ``a++ --b ⇔ ((a++)--) * b``
- This project goes with the second grouping when generating ASTs.

- To reiterate, it is **strongly discouraged** to write expressions similar to above example, the
  correct way to do so is either using the conventional `*` operator or parentheses to specify
  associativity.

# Output (parseTree.txt)
- The output is a simple visual display of the AST. Specifically, a pre-order tree-walk of the AST.
- An ASTNode is either a `TOK_LIT`, a `TOK_VAR`, or an operator surrounded by `""`.
- A node's child is placed below itself, preceed with a space and the symbol `|__`.
  - notice that 2 children are siblings (on the same level of the entire AST) if their `|__` symbols line up with each other.
  - As an example, the AST for expression `(a + b) / 2` will look like
  ```
	"/"
	 |__"+"
	  |__"a"
	  |__"b"
	 |__"2"
  ```

- Implicit mulitiplication will be handled by manually making an extra `*` node between the two operands.
	- Expressions`2 a` and `a 2` will thus become
	```
		"*"                "*"
		 |__"2"             |_"a"
		 |__"a"             |_"2"
	```


# Compile/Build Instructions
- If an `ExpressionTree` is built successfully, it will be printed onto a file named `parseTree.txt` in this directory.

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

# Design Goals/Direction
- see [design.md](design.md)

# Inspirations and References
1. https://github.com/PixelRifts/math-expr-evaluator/tree/master
2. https://craftinginterpreters.com
3. https://matklad.github.io/2020/04/13/simple-but-powerful-pratt-parsing.html

