# Compiling

To compile easymake manually, you must use a C compiler that supports the full C89 standard.
Don't worry, you are doing this proably for the first and the last time because easymake can build itself!

Here are instructions for compiling with some major compilers:
* tcc:   `tcc -o easymake src/*.c src/cutils/*.c -Iinclude -Iinclude/cutils -O3 -Os`
* gcc:   `gcc -o easymake src/*.c src/cutils/*.c -Iinclude -Iinclude/cutils -s -O3 -Os`
* clang: `clang -o easymake src/*.c src/cutils/*.c -Iinclude -Iinclude/cutils -O3 -Os`
* MSVC:  `cl src/*.c src/cutils/*.c /I include /I include/cutils /O2`
