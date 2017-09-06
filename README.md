# tuc

tuc is a small, experimental compiler designed for learning and experimenting. It compiles a really "special" language
called U.  So, naturally, the name tuc stands for The U Compiler.  U can stand for, in no particular order:

* useless
* uncreative
* unnecessary
* unimaginative
* unfortunate
* unnamed
* unable
* any other word of this kind you like

tuc translates U source code into assembly code for the [nasm](http://www.nasm.us/) assembler.

tuc is very primitive at the moment.  So, only simple mathematical expressions can be compiled. Hopefully, this will
change very soon.

An interested feature is that it is implemented in different languages. Currently, these are:

* C++
* Rust

**Note: tuc is not intended to have any useful purpose other than being a way to learn how compilers work.**

## Building

The process of building rust depends on which implementation you want to build. In all cases, however, the build
process will produce an executable called `tuc`.

### C++

1. `cd cpp`
2. `make`

### Rust

[TODO]

## Using

Once you've built tuc, compiling source code into assembly is very simple.  tuc only takes two arguments: the name of
a U source file and the name of the assembly file to be generated.

Suppose you created a U source file called `uncreativename.ul`.  Here is how you could build an executable for a 32-bit
Linux system using `nasm` and `ld`:

1. Compile the file to assembly code using `tuc uncreativename.ul uncreativename.asm`
2. Assemble the assembly code using `nasm -f elf32 uncreativename.asm -o uncreativename.o`
3. Link the assembled file into an executable using `ld uncreativename.o -o uncreativename`

From these instructions, you should be able to figure out what you need to do for whatever system you might have.
You can try using another "nasm compatible" assembler but your millage may vary.

## Testing

The `test/` directory contains some tests useful during development.

Unit tests for individual components of the program are in `test/unit_tests/`.  These use the Boost Test library so you
will need to have it installed in order to build the unit tests.

Actual tests of the compiler are in `test/compiler_tests`.  These are simple projects that can be commpiled using tuc.
They are useful to make sure the compiler actually compiles... correctly.  They also server as good examples of how to
use tuc.

## License

The source code for tuc is licensed under the MIT license and all accompanying documentation is licensed under a
<a rel="license" href="http://creativecommons.org/licenses/by/4.0/">Creative Commons Attribution 4.0 International License</a>.<br />
<a rel="license" href="http://creativecommons.org/licenses/by/4.0/">
    <img alt="Creative Commons License" style="border-width:0" src="https://i.creativecommons.org/l/by/4.0/88x31.png" />
</a><br />
