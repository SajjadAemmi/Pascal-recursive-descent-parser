# Pascal recursive descent parser

This program consists of two main phases: 
- Lexical Analyzer (find tokens)
- Parser (check syntax and create parse tree)

We used a recursive descent parser algorithm. This algorithm creates the tree in the Top Down method.

## Run
The desired simple program in Pascal language and a simple regular  expression for pascal are placed in the `IO/input` directory. Just run the following commands:
```
cmake -S . -B build
cmake --build build
./build/Pascal-recursive-descent-parser
```
The outputs will be saved in the `IO/output` directory.