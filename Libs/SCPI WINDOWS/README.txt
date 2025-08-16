This is the SCPI parser buildable in Windows.

The SCPI parser is a fairly complex module so this file serves as a test platform to debug it out of an embedded environment (not particularly amenable to debugging).

To build this for Windows you'll need to install MINGW or another GCC compiler.

Here's the build command:

> gcc -o scpi.exe scpi.c