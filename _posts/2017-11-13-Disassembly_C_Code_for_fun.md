---
layout: post
title: Disassembly C Code for fun
categories: Coding
author: Gustavo Walbon
---

.. read other post about [addr2line]({% post_url 2017-10-30-addr2line %})

```
vim trickline.c
  1 #include <stdio.h>
  2 
  3 #define TELLME printf ("This is line %d of file \"%s\" (function <%s>)\n",\
  4                       __LINE__, __FILE__, __func__)
  5 
  6 #line 1000 "thousand"
  7 void whereAmI ()
  8 {
  9     TELLME;
 10 }
 11 
 12 #line 999 "almost_one_thousand"
 13 int main ()
 14 {
 15     TELLME;
 16     whereAmI();
 17 
 18 #line 1 "reset_line"
 19     whereAmI();
 20     TELLME;
 21 
 22     printf("Today is %s %s.\n\n",__DATE__, __TIME__ );
 23     return 0;
 24 }
```
After you have created your program, compile it with debugsymbols:
`$ gcc -g -o trick.bin trickline.c`

iesult of the running trick.bin:
```
$ ./trick.bin
This is line 1001 of file "almost_one_thousand" (function <main>)
This is line 1002 of file "thousand" (function <whereAmI>)
This is line 1 of file "reset_line" (function <main>
This is line 1002 of file "thousand" (function <whereAmI>)
```
You can see that the correct number of lines and the file name have been changed by the macro
`#line <N> <FILENAME>`[[1]](https://gcc.gnu.org/onlinedocs/cpp/Standard-Predefined-Macros.html),
 watch out for that(I never saw that in kernel source).


Analyze its ASM code:
```
$ objdump -D trick.bin
...
00000000100006a4 <main>:
    100006a4:   02 10 40 3c     lis     r2,4098
    100006a8:   00 7f 42 38     addi    r2,r2,32512
    100006ac:   a6 02 08 7c     mflr    r0
    100006b0:   10 00 01 f8     std     r0,16(r1)
    100006b4:   f8 ff e1 fb     std     r31,-8(r1)
    100006b8:   91 ff 21 f8     stdu    r1,-112(r1)
    100006bc:   78 0b 3f 7c     mr      r31,r1
    100006c0:   fe ff 62 3c     addis   r3,r2,-2
    100006c4:   f0 89 63 38     addi    r3,r3,-30224
    100006c8:   e9 03 80 38     li      r4,1001
    100006cc:   fe ff a2 3c     addis   r5,r2,-2
    100006d0:   30 8a a5 38     addi    r5,r5,-30160
    100006d4:   fe ff c2 3c     addis   r6,r2,-2
    100006d8:   a0 8a c6 38     addi    r6,r6,-30048
    100006dc:   65 fd ff 4b     bl      10000440
<00000017.plt_call.printf@@GLIBC_2.17>
    100006e0:   18 00 41 e8     ld      r2,24(r1)
    100006e4:   69 ff ff 4b     bl      1000064c <whereAmI+0x8>
    100006e8:   65 ff ff 4b     bl      1000064c <whereAmI+0x8>
    100006ec:   fe ff 62 3c     addis   r3,r2,-2
    100006f0:   f0 89 63 38     addi    r3,r3,-30224
    100006f4:   02 00 80 38     li      r4,2
    100006f8:   fe ff a2 3c     addis   r5,r2,-2
    100006fc:   48 8a a5 38     addi    r5,r5,-30136
    10000700:   fe ff c2 3c     addis   r6,r2,-2
    10000704:   a0 8a c6 38     addi    r6,r6,-30048
    10000708:   39 fd ff 4b     bl      10000440
<00000017.plt_call.printf@@GLIBC_2.17>
    1000070c:   18 00 41 e8     ld      r2,24(r1)
    10000710:   fe ff 62 3c     addis   r3,r2,-2
    10000714:   58 8a 63 38     addi    r3,r3,-30120
    10000718:   fe ff 82 3c     addis   r4,r2,-2
    1000071c:   70 8a 84 38     addi    r4,r4,-30096
    10000720:   fe ff a2 3c     addis   r5,r2,-2
    10000724:   80 8a a5 38     addi    r5,r5,-30080
    10000728:   19 fd ff 4b     bl      10000440
<00000017.plt_call.printf@@GLIBC_2.17>
    1000072c:   18 00 41 e8     ld      r2,24(r1)
...

```

In this case you know where is which calling for a function, but in doubt you
can clain for a helping, `addr2line`;
```
$ addr2line -fip -e trick.bin 1000064c
whereAmI at /home/walbon/coding/thousand:1001
```
Do you see? The macro `#line <N> <FILENAME>` has changed the real number of
line and the filename, watch out for that.

# References
- [[1] https://gcc.gnu.org/onlinedocs/cpp/Standard-Predefined-Macros.html](https://gcc.gnu.org/onlinedocs/cpp/Standard-Predefined-Macros.html)

