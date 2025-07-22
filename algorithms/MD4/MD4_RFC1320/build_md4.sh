#!/bin/sh
# build_md4.sh - Compile MD4 with K&R support using gcc

gcc -std=c89 -Wno-deprecated-non-prototype -Wno-parentheses -Wno-pointer-sign -I. -DPROTOTYPES=1 -DMD=4 -o mddriver mddriver.c md4c.c
#gcc -std=gnu89 -Wno-deprecated-non-prototype -Wno-parentheses -Wno-pointer-sign -I. -DPROTOTYPES=1 -DMD=4 -o mddriver mddriver.c md4c.c

# Execute the MD4 driver program with a sample input
# The -s option is used to specify the input string
# The -t option is used for time trials
# The -x option is used to execute the program in a shell mode
./mddriver -shelloworld -t -x