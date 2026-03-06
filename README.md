# biblekernel
The BibleOS and derivates Core

It's a hybrid file system; the core is a microkernel, the security is provided by an exokernel combined with a nanokernel. The monolithic system works with kernel driver components and communication between POSIX processes (i.e., Windows NT PEPROCESS, PETHREADS, KTHREADS, EPROCESS, ETHREADS, and KPROCESS, QNX Threads, Sys-v/BSD Process (Procs and Threads), Linux Tasks (Proc, Cgroup, Tasks), Mach Process (Task, Thread), DOS Procs, Windows 9x VMM Kernel Threads and Process, Solaris LWPs, SEL4 VSpace + CSpaces + TCB + Scheduling Contexts (MCS)) and the file system.

Copyiright (c) Pedro Emanuel, 2026 Brazil 

## Faith and Respect

This project is created by a Christian developer. The name "Bible Kernel" and
biblical references are used respectfully.

Contributions that mock or criticize Christianity will not be accepted.
This is a condition of participation, regardless of the version (open or proprietary).

All are welcome to use and contribute, as long as this respect is maintained.

## advise

Learn more at:
file "LICENSE"
file "COPYING_ADVISE"
file "PROPIETARY"

## How do I compile the kernel? (This is only when the code is ready)

First, have the Makefile compile tools/config_kern.c

And after that, the makefile itself executes the compiled software that was previously the .c code in tools/config_kern.c

```text
make .conf.kern
```

The question will be: Which parts will be in ring 0, and which in ring 3? The parts within src/base/../, src/kfirmware, src/boot, src/abi, and src/api must be in ring 0. For usr/../, the user decides whether it's ring 3 or ring 0; in this context, usr doesn't stand for Unix System Resource but is an abbreviation for User.

Example output:
X=r0
A=r3...

## The CFLAGS (-D) instructions for the C preprocessor to set a macro (a constant or compilation flag) even before the code is compiled are:
-D_BK_KERNEL
-DKERNEL
-D_KERNEL
-DBIBLE
-DKERN...

## Build Kernel

Then, use:
```text
make kernel ARCH=X INC=../incl, ../base/../, ../kfirmware/inc/, ../api/, ../usr/incl OBJ_FMT=Y(ex:coff or elf) CONFIG=Z(REGEN(RE=RELEASE), REBIBLE, DEB(DEBUG), DEV(DEVELOPER)
```
to compile only the files in ring 0, and then link them into a single binary called `biblekernel.cae`.

## Build User Driver

If optional and you wish, you can compile the driver and file system software in user mode using 
```text
make usr
```

## HOW TO COMPACT THE KERNEL

To compress the kernel you will use: 
```text
make compress kernel
```

This compresses the kernel as follows: If the architecture is x86, PPC64, and/or ARM32, it will be compressed in Gzip;
if it is x64, ARM64, and/or RISC-V, it will be compressed in LZMA.

and then compiles the files inside ../descompress, and then links the files into a single binary called booz.
