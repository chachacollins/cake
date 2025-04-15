CC="clang"

BUILDFILES="main.c utils.c lexer.c debug.c parser.c eval.c"

COMPFLAGS="-Wpedantic -Wall -Wextra -Werror -std=gnu11 "

if [[ $1 == "release" ]]; then
    set -xe
    $CC -O3 $BUILDFILES -o cake $COMPFLAGS  
else
    set -xe
    $CC -ggdb $BUILDFILES -o cake $COMPFLAGS 
fi


