CC="clang"

BUILDFILES="src/main.c src/utils.c src/lexer.c src/debug.c src/parser.c src/eval.c"

COMPFLAGS="-Wpedantic -Wall -Wextra -Werror -std=gnu11 -Iinclude"

if [[ $1 == "release" ]]; then
    set -xe
    $CC -O3 $BUILDFILES -o cake $COMPFLAGS  
else
    set -xe
    $CC -ggdb $BUILDFILES -o cake $COMPFLAGS 
fi


