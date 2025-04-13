CC="clang"

BUILDFILES="main.c"

COMPFLAGS="-Wpedantic -Wall -Wextra -Werror -std=gnu11"

if [[ $1 == "release" ]]; then
    set -xe
    $CC -O3 $BUILDFILES -o cake $COMPFLAGS 
else
    set -xe
    $CC -ggdb $BUILDFILES -o cake $COMPFLAGS 
fi


