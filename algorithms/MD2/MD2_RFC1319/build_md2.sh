#!/bin/sh
# build_md2.sh - Compile MD2 with K&R support using gcc

gcc -std=c89 -Wno-deprecated-non-prototype -Wno-parentheses -Wno-pointer-sign -I. -DPROTOTYPES=1 -DMD=2 -o mddriver mddriver.c md2c.c
#gcc -std=gnu89 -Wno-deprecated-non-prototype -Wno-parentheses -Wno-pointer-sign -I. -DPROTOTYPES=1 -DMD=2 -o mddriver mddriver.c md2c.c

# Execute the MD2 driver program with a sample input
# The -s option is used to specify the input string
# The -t option is used for time trials
# The -x option is used to execute the program in a shell mode
./mddriver -shelloworld -t -x