 /*
 ** ********************************************************************
 ** md4driver.c -- sample routines to test **
 ** MD4 message digest algorithm. **
 ** Updated: 2/16/90 by Ronald L. Rivest **
 ** (C) 1990 RSA Data Security, Inc. **
 ** ********************************************************************
 */
 #include <stdio.h>
 #include "md4.h"
 /* MDtimetrial()
 ** A time trial routine, to measure the speed of MD4.
 ** Measures speed for 1M blocks = 64M bytes.
 */
 MDtimetrial()
 { unsigned int X[16];
 MDstruct MD;
 int i;
 double t;
 for (i=0;i<16;i++) X[i] = 0x01234567 + i;
 printf
 ("MD4 time trial. Processing 1 million 64-character blocks...\n");
 clock();
 MDbegin(&MD);
 for (i=0;i<1000000;i++) MDupdate(&MD,X,512);
 MDupdate(&MD,X,0);
 t = (double) clock(); /* in microseconds */
 MDprint(&MD); printf(" is digest of 64M byte test input.\n");
 printf("Seconds to process test input: %g\n",t/1e6);
 printf("Characters processed per second: %ld.\n",(int)(64e12/t));
 }
 /* MDstring(s)
 ** Computes the message digest for string s.
 ** Prints out message digest, a space, the string (in quotes) and a
 ** carriage return.
 */
 MDstring(s)
 unsigned char *s;
 { unsigned int i, len = strlen(s);
 MDstruct MD;
 MDbegin(&MD);
 for (i=0;i+64<=len;i=i+64) MDupdate(&MD,s+i,512);
 MDupdate(&MD,s+i,(len-i)*8);
 MDprint(&MD);
 printf(" \"%s\"\n",s);
 }
 /* MDfile(filename)
 ** Computes the message digest for a specified file.
 ** Prints out message digest, a space, the file name, and a
 ** carriage return.
 */
 MDfile(filename)
 char *filename;
 { FILE *f = fopen(filename,"rb");
 unsigned char X[64];
 MDstruct MD;
 int b;
 if (f == NULL)
 { printf("%s can't be opened.\n",filename); return; }
 MDbegin(&MD);
 while ((b=fread(X,1,64,f))!=0) MDupdate(&MD,X,b*8);
 MDupdate(&MD,X,0);
 MDprint(&MD);
 printf(" %s\n",filename);
 fclose(f);
 }
 /* MDfilter()
 ** Writes the message digest of the data from stdin onto stdout,
 ** followed by a carriage return.
 */
 MDfilter()
 { unsigned char X[64];
 MDstruct MD;
 int b;
 MDbegin(&MD);
 while ((b=fread(X,1,64,stdin))!=0) MDupdate(&MD,X,b*8);
 MDupdate(&MD,X,0);
 MDprint(&MD);
 printf("\n");
 }
 /* MDtestsuite()
 ** Run a standard suite of test data.
 */
 MDtestsuite()
 {
 printf("MD4 test suite results:\n");
 MDstring("");
 MDstring("a");
 MDstring("abc");
 MDstring("message digest");
 MDstring("abcdefghijklmnopqrstuvwxyz");
 MDstring
 ("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789");
 MDfile("foo"); /* Contents of file foo are "abc" */
 }
 main(argc,argv)
 int argc;
 char *argv[];
 { int i;
 /* For each command line argument in turn:
 ** filename -- prints message digest and name of file
 ** -sstring -- prints message digest and contents of string
 ** -t -- prints time trial statistics for 64M bytes
 ** -x -- execute a standard suite of test data
 ** (no args) -- writes messages digest of stdin onto stdout
 */
 if (argc==1) MDfilter();
 else
 for (i=1;i<argc;i++)
 if (argv[i][0]=='-' && argv[i][1]=='s') MDstring(argv[i]+2);
 else if (strcmp(argv[i],"-t")==0) MDtimetrial();
 else if (strcmp(argv[i],"-x")==0) MDtestsuite();
 else MDfile(argv[i]);
 }
 /*
 ** end of md4driver.c
 ****************************(cut)***********************************/

/*
cc -o md4 -O4 md4.c md4driver.c
 md4.c:
 md4driver.c:
 Linking:
 >
 >md4 -x
 MD4 test suite results:
 31d6cfe0d16ae931b73c59d7e0c089c0 ""
 bde52cb31de33e46245e05fbdbd6fb24 "a"
 a448017aaf21d8525fc10ae87aa6729d "abc"
 d9130a8164549fe818874806e1c7014b "message digest"
 d79e1c308aa5bbcdeea8ed63df412da9 "abcdefghijklmnopqrstuvwxyz"
 043f8582f241db351ce627e153e7f0e4
 "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"
 a448017aaf21d8525fc10ae87aa6729d abcfile
 >
 >md4 -sabc -shi
 a448017aaf21d8525fc10ae87aa6729d "abc"
 cfaee2512bd25eb033236f0cd054e308 "hi"
 >
 >md4 *
 a448017aaf21d8525fc10ae87aa6729d abcfile
 d316f994da0e951cf9502928a1f73300 md4
 379adb39eada0dfdbbdfdcd0d9def8c4 md4.c
 9a3f73327c65954198b1f45a3aa12665 md4.doc
 37fe165ac177b461ff78b86d10e4ff33 md4.h
 7dcba2e2dc4d8f1408d08beb17dabb2a md4.o
 08790161bfddc6f5788b4353875cb1c3 md4driver.c
 1f84a7f690b0545d2d0480d5d3c26eea md4driver.o
 >
 >cat abcfile | md4
 a448017aaf21d8525fc10ae87aa6729d
 >
 >md4 -t
 MD4 time trial. Processing 1 million 64-character blocks...
 6325bf77e5891c7c0d8104b64cc6e9ef is digest of 64M byte test input.
 Seconds to process test input: 44.0982
 Characters processed per second: 1451305.
 >
 >
 ------------------------ end of sample session --------------------*/