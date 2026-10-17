# Optimizing Expression Compiler & Register Allocator in C++17

A multi-pass compiler that translates a straight-line imperative language into Three-Address Code (TAC), performs linear-scan register allocation with spill handling, and emits target assembly.

## Pipeline

1. **Lexer** converts source text into tokens and records line and column numbers.
2. **Parser** uses recursive descent to build an AST.
3. **Optimizer** folds constant integer arithmetic, such as `4 + 2 * 5` into `14`.
4. **TAC generator** lowers expressions into simple three-address instructions.
5. **Register allocator** assigns live values to `R0`–`R3` and uses spill slots
   when more than four values are live.
6. **Assembly emitter** writes instructions using the allocated locations.

There is no control flow, so the allocator uses straight-line live ranges from
the first occurrence of a value to its last use.

## Language

The language supports:

- Integer literals and variable names
- `let` declarations and assignments, each ending in `;`
- Parenthesized expressions
- Integer arithmetic with `+`, `-`, `*`, and `/`

Multiplication and division have higher precedence than addition and
subtraction. Operators at the same precedence level associate from left to
right.

Example program:

```text
let answer = 4 + 2 * 5;
let next = answer + 1;
```

The language does not currently support control flow, unary operators,
comparisons, comments, or checking that variables have been declared.

## Build

Requires CMake 3.16 or newer and a C++17 compiler.

```sh
cmake -S . -B build
cmake --build build
```

## Run

Pass a source file, or pipe source through standard input. The default output
is pseudo-assembly:

```sh
build/compiler example.imp
printf 'let answer = 4 + 2 * 5;\nlet next = answer + 1;\n' | build/compiler
```

The example emits:

```text
LOADI R0, #14
ADD R1, R0, #1
MOV R0, R1
```

`R0` holds `answer`; after its last use, the allocator reuses `R0` for `next`.
The output is a teaching format, not machine code for a specific processor.
Spilled values use locations like `[spill0]`, which the pseudo instructions
can read or write directly.

## Inspect compiler stages

```sh
build/compiler --tokens example.imp  # lexer output
build/compiler --ir example.imp      # optimized TAC
build/compiler --alloc example.imp   # assembly plus live ranges on stderr
build/compiler --help
```

For the example above, `--ir` prints:

```text
answer = 14
%t0 = answer + 1
next = %t0
```

`--tokens` cannot be combined with `--ir` or `--alloc`. `--alloc` can be
combined with `--ir`; allocation details go to standard error and TAC goes to
standard output.

## Run tests

```sh
ctest --test-dir build --output-on-failure
```

The test suite covers token locations, expression precedence, constant
folding, TAC generation, register conflicts, spilling, assembly output, and
syntax errors.
