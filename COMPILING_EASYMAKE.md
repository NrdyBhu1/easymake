# Compiling

To compile easymake manually, you must do so using either tcc, gcc, clang or MSVC.
Don't worry, you are doing this proably for the first and the last time because easymake can build itself!

Here are instructions for compiling with either of those four compilers:
* tcc:   `tcc src/*.c -Iinclude -o easymake`
* gcc:   `gcc src/*.c -Iinclude -s -O3 -o easymake`
* clang: `clang src/*.c -Iinclude -O3 -o easymake`
* MSVC:  `cl src/*.c /I include /O2`