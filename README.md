about `cygwin_stackdump()`:

* `cygwin_stackdump()` creates a file named executable's plus suffixed `.stackdump`.
* `cygwin_stackdump()` writes simple stack trace which are constructed by frame,  
  function and args in hex notation.
* `fopen()` stackdump file immediately after `cygwin_stackdump()` calling in signle  
  process will fail, so use `fork()` and `waitpid()`.
* cygwin has clang-3.1 as most recent version.  
  w/ 3.1, DWARF part looks broken, so unable to use `addr2line` to decode.
* do not use `cygwin_stackdump()` for new code.  
  Christopher Faylor said:  
  "Exporting this function is an ancient mistake but we are stuck with  
   keeping it around to maintain backwards compatibility.  There is  
   no guarantee that the stackdump you get from this is correct and  
   we have no plans on augmenting it further."  
  http://cygwin.com/ml/cygwin/2014-01/msg00056.html

about `unwind.h`:

* cygwin has clang-3.1 as most recent version.  
  w/ clang-3.1, DWARF part looks broken, so unable to use `addr2line` to decode.  
  w/ clang-3.1, only `_Unwind_GetIP()` and `_Unwind_Backtrace()` are supported.  
  as upstream version, see http://clang.llvm.org/doxygen/unwind_8h.html.

```
$ uname -a
CYGWIN_NT-5.1 jabberwocky 1.7.27(0.271/5/3) 2013-12-09 11:57 i686 Cygwin

$ make CC=gcc info clean check
gcc -v
Using built-in specs.
COLLECT_GCC=gcc
COLLECT_LTO_WRAPPER=/usr/lib/gcc/i686-pc-cygwin/4.8.2/lto-wrapper.exe
Target: i686-pc-cygwin
Configured with: /cygdrive/i/szsz/tmpp/gcc4/gcc-4.8.2-2/src/gcc-4.8.2/configure --srcdir=/cygdrive/i/szsz/tmpp/gcc4/gcc-4.8.2-2/src/gcc-4.8.2 --prefix=/usr --exec-prefix=/usr --bindir=/usr/bin --sbindir=/usr/sbin --libexecdir=/usr/libexec --datadir=/usr/share --localstatedir=/var --sysconfdir=/etc --libdir=/usr/lib --datarootdir=/usr/share --docdir=/usr/share/doc/gcc --htmldir=/usr/share/doc/gcc/html -C --build=i686-pc-cygwin --host=i686-pc-cygwin --target=i686-pc-cygwin --without-libiconv-prefix --without-libintl-prefix --enable-shared --enable-shared-libgcc --enable-static --enable-version-specific-runtime-libs --enable-bootstrap --disable-__cxa_atexit --with-dwarf2 --with-arch=i686 --with-tune=generic --disable-sjlj-exceptions --enable-languages=ada,c,c++,fortran,java,lto,objc,obj-c++ --enable-graphite --enable-threads=posix --enable-libatomic --enable-libgomp --disable-libitm --enable-libquadmath --enable-libquadmath-support --enable-libssp --enable-libada --enable-libjava --enable-libgcj-sublibs --disable-java-awt --disable-symvers --with-ecj-jar=/usr/share/java/ecj.jar --with-gnu-ld --with-gnu-as --with-cloog-include=/usr/include/cloog-isl --without-libiconv-prefix --without-libintl-prefix --with-system-zlib --libexecdir=/usr/lib
Thread model: posix
gcc version 4.8.2 (GCC)
rm -f test-cygwin_stackdump.exe
rm -f test-cygwin_stackdump.exe.stackdump
rm -f test-cygwin_stackdump-fork-waitpid.exe
rm -f test-cygwin_stackdump-fork-waitpid.exe.stackdump
rm -f test-unwind.exe
gcc -DUSE_CYGWIN_STACKDUMP -g -O0 -std=gnu99 -Wall -funwind-tables -o test-cygwin_stackdump.exe test.c
gcc -DUSE_CYGWIN_STACKDUMP -DUSE_FORK_WAITPID -g -O0 -std=gnu99 -Wall -funwind-tables -o test-cygwin_stackdump-fork-waitpid.exe test.c
gcc -DUSE_UNWIND -g -O0 -std=gnu99 -Wall -funwind-tables -o test-unwind.exe test.c
./test-cygwin_stackdump.exe || true
fopen: Device or resource busy
/bin/sh: line 1:  3504 Aborted                 (core dumped) ./test-cygwin_stackdump.exe
./test-cygwin_stackdump.exe 2>&1 | awk '/^[0-9]/ { print $2 }' | addr2line -f -e ./test-cygwin_stackdump.exe || true
./test-cygwin_stackdump-fork-waitpid.exe
Stack trace:
Frame     Function  Args
0022ABC8  00401374 (20000008, 00401A10, 0022ABE8, 00401390)
0022ABD8  00401382 (00000000, 00000000, 0022ABF8, 0040139E)
0022ABE8  00401390 (00401A10, 00401A10, 0022AC08, 004013AC)
0022ABF8  0040139E (611882B0, 7FFDFC00, 0022AC18, 004013BA)
0022AC08  004013AC (7C80B760, 00000000, 0022AC28, 004013C8)
0022AC18  004013BA (00000000, 610063A0, 0022AC38, 004013D6)
0022AC28  004013C8 (00401A14, 00401A10, 0022AC48, 004013E4)
0022AC38  004013D6 (610063A0, 6119EF5B, 0022AC58, 004013F2)
0022AC48  004013E4 (0000002F, 0022AC8C, 0022AC68, 0040140F)
0022AC58  004013F2 (00000000, 0022CDB4, 0022AD28, 61007FB5)
0022AC68  0040140F (00000001, 0022AC8C, 20010100, 61007F58)
0022AD28  61007FB5 (00000000, 0022CDB4, 61007120, 00000000)
End of stack trace
./test-cygwin_stackdump-fork-waitpid.exe 2>&1 | awk '/^[0-9]/ { print $2 }' | addr2line -f -e ./test-cygwin_stackdump-fork-waitpid.exe
f9
/home/hiyuh/git-repos/cygwin-stackdump-example/test.c:95
f8
/home/hiyuh/git-repos/cygwin-stackdump-example/test.c:96
f7
/home/hiyuh/git-repos/cygwin-stackdump-example/test.c:97
f6
/home/hiyuh/git-repos/cygwin-stackdump-example/test.c:98
f5
/home/hiyuh/git-repos/cygwin-stackdump-example/test.c:99
f4
/home/hiyuh/git-repos/cygwin-stackdump-example/test.c:100
f3
/home/hiyuh/git-repos/cygwin-stackdump-example/test.c:101
f2
/home/hiyuh/git-repos/cygwin-stackdump-example/test.c:102
f1
/home/hiyuh/git-repos/cygwin-stackdump-example/test.c:103
f0
/home/hiyuh/git-repos/cygwin-stackdump-example/test.c:104
main
/home/hiyuh/git-repos/cygwin-stackdump-example/test.c:111
??
??:0
./test-unwind.exe
#0: 00401209
#1: 00401216
#2: 00401224
#3: 00401232
#4: 00401240
#5: 0040124e
#6: 0040125c
#7: 0040126a
#8: 00401278
#9: 00401286
#10: 00401294
#11: 004012a2
#12: 004012b5
#13: 61007fb5
./test-unwind.exe           2>&1 | awk          '{ print $2 }' | addr2line -f -e ./test-unwind.exe
show_stackdump
/home/hiyuh/git-repos/cygwin-stackdump-example/test.c:83
f10
/home/hiyuh/git-repos/cygwin-stackdump-example/test.c:88
f9
/home/hiyuh/git-repos/cygwin-stackdump-example/test.c:95
f8
/home/hiyuh/git-repos/cygwin-stackdump-example/test.c:96
f7
/home/hiyuh/git-repos/cygwin-stackdump-example/test.c:97
f6
/home/hiyuh/git-repos/cygwin-stackdump-example/test.c:98
f5
/home/hiyuh/git-repos/cygwin-stackdump-example/test.c:99
f4
/home/hiyuh/git-repos/cygwin-stackdump-example/test.c:100
f3
/home/hiyuh/git-repos/cygwin-stackdump-example/test.c:101
f2
/home/hiyuh/git-repos/cygwin-stackdump-example/test.c:102
f1
/home/hiyuh/git-repos/cygwin-stackdump-example/test.c:103
f0
/home/hiyuh/git-repos/cygwin-stackdump-example/test.c:104
main
/home/hiyuh/git-repos/cygwin-stackdump-example/test.c:111
??
??:0

 $ make CC=clang info clean check
clang -v
clang version 3.1 (branches/release_31)
Target: i386-pc-cygwin
Thread model: posix
rm -f test-cygwin_stackdump.exe
rm -f test-cygwin_stackdump.exe.stackdump
rm -f test-cygwin_stackdump-fork-waitpid.exe
rm -f test-cygwin_stackdump-fork-waitpid.exe.stackdump
rm -f test-unwind.exe
clang -DUSE_CYGWIN_STACKDUMP -g -O0 -std=gnu99 -Wall -funwind-tables -o test-cygwin_stackdump.exe test.c
clang -DUSE_CYGWIN_STACKDUMP -DUSE_FORK_WAITPID -g -O0 -std=gnu99 -Wall -funwind-tables -o test-cygwin_stackdump-fork-waitpid.exe test.c
clang -DUSE_UNWIND -g -O0 -std=gnu99 -Wall -funwind-tables -o test-unwind.exe test.c
./test-cygwin_stackdump.exe || true
fopen: Device or resource busy
/bin/sh: line 1:  3412 Aborted                 (core dumped) ./test-cygwin_stackdump.exe
./test-cygwin_stackdump.exe 2>&1 | awk '/^[0-9]/ { print $2 }' | addr2line -f -e ./test-cygwin_stackdump.exe || true
./test-cygwin_stackdump-fork-waitpid.exe
Stack trace:
Frame     Function  Args
0022AC0C  00401268 (0022AC1C, 00401248, 0022AC24, 00401238)
0022AC14  00401258 (0022AC24, 00401238, 0022AC2C, 00401228)
0022AC1C  00401248 (0022AC2C, 00401228, 0022AC34, 00401218)
0022AC24  00401238 (0022AC34, 00401218, 0022AC3C, 00401208)
0022AC2C  00401228 (0022AC3C, 00401208, 0022AC44, 004011F8)
0022AC34  00401218 (0022AC44, 004011F8, 0022AC4C, 004011E8)
0022AC3C  00401208 (0022AC4C, 004011E8, 0022AC54, 004011D8)
0022AC44  004011F8 (0022AC54, 004011D8, 0022AC68, 004011BD)
0022AC4C  004011E8 (0022AC68, 004011BD, 0022AC8C, 00000001)
0022AC54  004011D8 (0022AC8C, 00000001, 00000000, 0022AD28)
0022AC68  004011BD (00000001, 0022AC8C, 20010100, 61007F58)
0022AD28  61007FB5 (00000000, 0022CDB4, 61007120, 00000000)
End of stack trace
./test-cygwin_stackdump-fork-waitpid.exe 2>&1 | awk '/^[0-9]/ { print $2 }' | addr2line -f -e ./test-cygwin_stackdump-fork-waitpid.exe
BFD: Dwarf Error: Offset (4263950) greater than or equal to .debug_str size (1405).
BFD: Dwarf Error: Offset (4263990) greater than or equal to .debug_str size (1405).
BFD: Dwarf Error: Offset (4263997) greater than or equal to .debug_str size (1405).
BFD: Dwarf Error: Offset (4260301) greater than or equal to .debug_line size (3475).
??
fake:?
??
fake:?
??
fake:?
??
fake:?
??
fake:?
??
fake:?
??
fake:?
??
fake:?
??
fake:?
??
fake:?
??
fake:?
??
??:0
./test-unwind.exe
#0: 00401312
#1: 004012e8
#2: 004012d8
#3: 004012c8
#4: 004012b8
#5: 004012a8
#6: 00401298
#7: 00401288
#8: 00401278
#9: 00401268
#10: 00401258
#11: 00401248
#12: 00401233
#13: 61007fb5
./test-unwind.exe           2>&1 | awk          '{ print $2 }' | addr2line -f -e ./test-unwind.exe
BFD: Dwarf Error: Offset (4263950) greater than or equal to .debug_str size (480).
BFD: Dwarf Error: Offset (4263990) greater than or equal to .debug_str size (480).
BFD: Dwarf Error: Offset (4263997) greater than or equal to .debug_str size (480).
BFD: Dwarf Error: Offset (4260301) greater than or equal to .debug_line size (3423).
??
fake:?
??
fake:?
??
fake:?
??
fake:?
??
fake:?
??
fake:?
??
fake:?
??
fake:?
??
fake:?
??
fake:?
??
fake:?
??
fake:?
??
fake:?
??
??:0
```
