# kotlin-arm64
A kotlin to ARM64 compiler

## Run the Compiler
```zsh
cc -std=c11 -Wall -Wextra -Werror -Wpedantic -pedan
tic-errors -g -o main assembly.c && ./main
```

## Compiler the assembly code (MacOS M*)
The easiest way:
```zsh
clang -o helloworld helloworld.s -arch arm64
```

The manual way:
```zsh
as -o helloworld.o helloworld.s -arch arm64
ld helloworld.o -o helloworld -l System -syslibroot `xcrun -sdk macosx --show-sdk-path`
```

## Run the program
```zsh
./helloworld
```

