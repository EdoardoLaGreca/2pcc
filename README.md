# 2pcc — the two-phase C compiler

You can either pronounce it as *two-pee-see-see* or *two pieces*.

## Introduction

Typically, making an executable file out of source code involves a compiler, which translates the source code into object code, and a linker, which links the object code with prospective libraries. 2pcc takes a different approach: it divides the job into platform-*dependent* and platform-*independent*, so that every part does roughly the same amount of work. This allows each part to focus on one job only and therefore achieve simplicity. Platform-independent work is done by a *representer* while platform-dependent work is done by a *realizer*.

The 2pcc linker also attempts to statically link shared libraries, so that it is not necessary to build them again. Why? Because [dynamic linking was a mistake](http://harmful.cat-v.org/software/dynamic-linking/).

## Documentation

You can find some documentation in the `doc` directory.

## Roadmap

☑ = present, tested and working.  
☒ = missing, not tested or not working.

### Representer

 - ☒ Line reconstruction
 - ☒ Preprocessor
 - ☒ Replacements
   + ☒ Type replacement
   + ☒ IR code generation
   + ☒ Expression validity check and replacement
 - ☒ Optimization (optional)
 - ☒ UTF-8 support

### Realizer

Each realizer has its own roadmap since its development is platform-dependent.

## Goals

 - Simplicity
 - Lightness
 - Minimalism

## Non-goals

 - Complexity
 - Heavyweight features
 - Extreme code optimization

## License

![CC0 logo](https://mirrors.creativecommons.org/presskit/buttons/88x31/svg/cc-zero.svg)

2pcc is licensed under the [Creative Commons Zero](https://en.wikipedia.org/wiki/Creative_Commons_license#Zero_/_public_domain) (CC0) license.
