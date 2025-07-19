#!/bin/sh
# build_md2.sh - Compile MD2 with K&R support using gcc

gcc -std=c89 -Wno-deprecated-non-prototype -o md2_main md2_main.c
#gcc -std=gnu89 -Wno-deprecated-non-prototype -o md2_main md2_main.c

# Execute the MD2 main program with a sample input
./md2_main "helloworld"
