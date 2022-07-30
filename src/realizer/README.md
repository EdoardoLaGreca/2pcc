# Realizers

This is the directory where realizers are placed.

## Naming conventions

The names are made as follows

```
ARCH_OS
```

where `ARCH` is the CPU architecture and `OS` is the operating system. Both the two names must be made of *alphanumeric lowercase characters* only (`[a-z0-9]`). If the name happens to have characters other than the ones mentioned, one must find an alternative name (e.g. "x86_64" becomes "amd64").

For example, in case of Linux on x86_64, the name is:

```
linux_amd64
```

Another example - 9front on RISC-V 64 bits:

```
9front_riscv64
```

## Structural conventions

In order to insure that the use of realizers is painless, they must have common or similar user interfaces and they must be able to be used even in other platforms. These goals can be achieved by setting some operating rules, which are explained in the following paragraphs.

Realizers must be formed by *one* executable file only and it must be *as portable as possible* to enable cross-compilation.

Realizers are required to implement a common command-line user interface. The usage should look like this

```
ARCH_OS [-l linkfile] file ...
```

the options have the following meanings:

 - `-l [linkfile]` Specifies the link file, which is a file that contains the paths of the libraries to be linked, each in a separate line. If not written, no libraries are linked.

Realizers may have additional parameters if required due to the platform.
