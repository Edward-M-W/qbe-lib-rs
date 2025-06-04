# QBE as a (rust) library

The upstream [QBE repository](https://c9x.me/compile/code.html) does not expose
any way of using QBE as a library. This offers the ability to call QBE as
library by more or less duplicating the existing main.c as lib.c and replacing
the main function with a function 'qbeEmit'.

A tiny wrapper around 'qbeEmit' is then provided via src/lib.rs

This library is based on [this one](https://github.com/malcolmstill/qbe-zig)
for zig.

## Use

There is an example in [example/](./example/) of how you can use this.
