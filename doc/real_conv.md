# Realizer conventions

## Naming conventions

The names are made as follows

```
ARCH_OS
```

where `ARCH` is the CPU architecture and `OS` is the operating system. Both the two names must be made of *alphanumeric lowercase characters* only (`[a-z0-9]`). If the name happens to have characters other than the ones mentioned, one must find an alternative name (e.g. "x86_64" becomes "amd64").

For example, in case of OpenBSD on x86_64, the name is:

```
openbsd_amd64
```

Another example - 9front on RISC-V 64 bits:

```
plan9_riscv64
```

### Variations of the same platform

Instances of the same platform may have substantial system-level differences. Linux is an emblematic case: two Linux distributions may have, for example, different C library implementations (glibc or musl, just to name a few). Those differences affect the final executable code, so they have to be taken in consideration.

For this purpose, there isn't a real convention. Follow the conventions adopted by other realizers for the same platform or, if it's the first realizer for that platform, come up with your own. However, it is important to add the details *after* `OS_ARCH`, not before or in between. For example:

```
linux_amd64_glibc
```

## Structural conventions

In order to insure that the use of realizers is painless, they must have common or similar user interfaces and they must be able to be used even in other platforms. These goals can be achieved by setting some operating rules, which are explained in the following paragraphs.

Realizers must be formed by *one executable file only* and it must be *as portable as possible* to enable cross-compilation.

Realizers are required to implement a common command-line user interface. The usage should look like this

```
ARCH_OS [-l linkfile] file ...
```

and the options have the following meanings:

 - `-l [linkfile]` Specifies the link file, which is a file that contains the paths of the libraries to be linked, each in a separate line. If not written, no libraries are linked.

Realizers may have additional parameters if required due to the platform.
