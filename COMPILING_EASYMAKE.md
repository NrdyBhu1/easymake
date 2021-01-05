# Compiling

To compile easymake manually, you must use a C compiler that supports the full C89 standard.
Don't worry, you are probably doing this for the first and the last time because easymake can build itself!

Here are instructions for compiling with some major compilers:
* tcc:   `tcc -o easymake src/*.c -Iinclude -O3 -Os`
* gcc:   `gcc -o easymake src/*.c -Iinclude -s -O3 -Os`
* clang: `clang -o easymake src/*.c -Iinclude -O3 -Os`
* MSVC:  `cl src/*.c /I include /O2`
