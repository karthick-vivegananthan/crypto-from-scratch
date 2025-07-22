#!/bin/sh
# build_md4.sh - Compile MD4 with K&R support using gcc

#set define LOWBYTEFIRST TRUE
gcc -std=c89 -Wno-deprecated-non-prototype -Wno-implicit-function-declaration -Wno-return-type -Wno-pointer-sign -Wno-format -DLOWBYTEFIRST=1 -I. -o md4driver md4driver.c md4.c
#gcc -std=gnu89 -Wno-deprecated-non-prototype -Wno-implicit-function-declaration -Wno-return-type -Wno-pointer-sign -Wno-format -DLOWBYTEFIRST=1 -I. -o md4driver md4driver.c md4.c

# Execute the MD4 driver program with a sample input
# The -s option is used to specify the input string
# The -t option is used for time trials
# The -x option is used to execute the program in a shell mode
./md4driver -shelloworld -t -x