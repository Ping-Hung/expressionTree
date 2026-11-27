# Repository Guidelines

## Project Structure

ExpressionTree is a C17 command-line parser that reads an arithmetic expression from standard input and writes its abstract syntax tree (AST) to `parseTree.txt`. `main.c` handles input and output. `src/tokenizer.c` groups input into tokens, and `src/expression_tree.c` builds, prints, and frees the AST. `headers/` contains module declarations and inline parser helpers. See `README.md` for the grammar and expected output, `design.md` for design notes, and `images/` for diagrams.

## Build and Development Commands

Run these commands from the repository root with GCC and Make installed.

- `make` builds `expressionTree` with `-std=c17 -Wall -Werror -Wvla -pedantic -g`.
- `./expressionTree` runs the CLI and prompts for an expression.
- `make debug` rebuilds the executable with `DEBUG` defined to show tokenizer output.
- `make valgrind` runs the CLI with leak checking and origin tracking. It requires Valgrind.
- `make clean` removes the executable.

## Coding Style

Follow `.editorconfig` for new and edited code. Use eight-space indentation, UTF-8, and LF line endings. Align `case` labels with `switch` and indent their bodies. Follow existing naming patterns such as `tokenizer_tokenize` for module functions, `ExpressionTree` for types, and `TOK_LIT` for token constants. Use the compiler checks in `make` to catch warnings.

## Testing

`make test` builds and launches the interactive CLI. The repository has no automated test suite or coverage threshold. For a repeatable smoke check, run `printf '%s\n' '(a + b) / 2' | make test` and verify that `parseTree.txt` has `/` at the root, `+` on the left, and `2` on the right. For parser changes, exercise precedence, implicit multiplication, prefix and postfix operators, and malformed input as applicable.

## Commits and Pull Requests

Recent commit subjects are short and specific, such as `Makefile slight syntax update`. Keep each commit focused. In pull requests, describe the behavior changed, list the commands and inputs used to verify it, and link related issues when available. Include representative AST output when the tree shape changes.
