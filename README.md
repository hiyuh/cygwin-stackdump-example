about `cygwin_stackdump()`:

* `cygwin_stackdump()` creates a file named executable's plus suffixed `.stackdump`.
* `cygwin_stackdump()` writes simple stack trace which are constructed by frame,  
  function and args in hex notation.
* `fopen()` stackdump file immediately after `cygwin_stackdump()` calling in signle  
  process will fail, so use fork() and waitpid().
* cygwin has clang-3.1 as most recent version.  
  w/ 3.1, DWARF part looks broken, so unable to use `addr2line` to decode.

about `unwind.h`:

* cygwin has clang-3.1 as most recent version.  
  w/ clang-3.1, DWARF part looks broken, so unable to use `addr2line` to decode.  
  w/ clang-3.1, only `_Unwind_GetIP()` and `_Unwind_Backtrace()` are supported.  
  as upstream version, see `http://clang.llvm.org/doxygen/unwind_8h.html.`

```
$ uname -a
CYGWIN_NT-6.1-WOW64 lynx 1.7.27(0.271/5/3) 2013-12-09 11:57 i686 Cygwin

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
rm -f test-unwind.exe
gcc -DUSE_CYGWIN_STACKDUMP -g -O0 -std=gnu99 -Wall -funwind-tables -o test-cygwin_stackdump.exe test.c
gcc -DUSE_UNWIND -g -O0 -std=gnu99 -Wall -funwind-tables -o test-unwind.exe test.c
./test-cygwin_stackdump.exe
Stack trace:
Frame     Function  Args
0028AB98  0040138D (80000008, 00401A10, 0028ABB8, 004013A9)
0028ABA8  0040139B (00000000, 00000000, 0028ABC8, 004013B7)
0028ABB8  004013A9 (00401A10, 00401A10, 0028ABD8, 004013C5)
0028ABC8  004013B7 (611882B0, 763C2EAD, 0028ABE8, 004013D3)
0028ABD8  004013C5 (00000000, 00000000, 0028ABF8, 004013E1)
0028ABE8  004013D3 (00000000, 610063A0, 0028AC08, 004013EF)
0028ABF8  004013E1 (00401A14, 00401A10, 0028AC18, 004013FD)
0028AC08  004013EF (610063A0, 6119EF5B, 0028AC28, 0040140B)
0028AC18  004013FD (0000002F, 0028AC5C, 0028AC38, 0040141E)
0028AC28  0040140B (00000000, 0028CD84, 0028ACF8, 61007FB5)
0028AC38  0040141E (00000001, 0028AC5C, 80010100, 61007F58)
0028ACF8  61007FB5 (00000000, 0028CD84, 61007120, 00000000)
End of stack trace
./test-cygwin_stackdump.exe 2>&1 | awk '/^[0-9]/ { print $2 }' | addr2line -f -e ./test-cygwin_stackdump.exe
f9
/home/hiyuh/git-repos/cygwin-stackdump-example/test.c:76
f8
/home/hiyuh/git-repos/cygwin-stackdump-example/test.c:77
f7
/home/hiyuh/git-repos/cygwin-stackdump-example/test.c:78
f6
/home/hiyuh/git-repos/cygwin-stackdump-example/test.c:79
f5
/home/hiyuh/git-repos/cygwin-stackdump-example/test.c:80
f4
/home/hiyuh/git-repos/cygwin-stackdump-example/test.c:81
f3
/home/hiyuh/git-repos/cygwin-stackdump-example/test.c:82
f2
/home/hiyuh/git-repos/cygwin-stackdump-example/test.c:83
f1
/home/hiyuh/git-repos/cygwin-stackdump-example/test.c:84
f0
/home/hiyuh/git-repos/cygwin-stackdump-example/test.c:85
main
/home/hiyuh/git-repos/cygwin-stackdump-example/test.c:89
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
/home/hiyuh/git-repos/cygwin-stackdump-example/test.c:64
f10
/home/hiyuh/git-repos/cygwin-stackdump-example/test.c:69
f9
/home/hiyuh/git-repos/cygwin-stackdump-example/test.c:76
f8
/home/hiyuh/git-repos/cygwin-stackdump-example/test.c:77
f7
/home/hiyuh/git-repos/cygwin-stackdump-example/test.c:78
f6
/home/hiyuh/git-repos/cygwin-stackdump-example/test.c:79
f5
/home/hiyuh/git-repos/cygwin-stackdump-example/test.c:80
f4
/home/hiyuh/git-repos/cygwin-stackdump-example/test.c:81
f3
/home/hiyuh/git-repos/cygwin-stackdump-example/test.c:82
f2
/home/hiyuh/git-repos/cygwin-stackdump-example/test.c:83
f1
/home/hiyuh/git-repos/cygwin-stackdump-example/test.c:84
f0
/home/hiyuh/git-repos/cygwin-stackdump-example/test.c:85
main
/home/hiyuh/git-repos/cygwin-stackdump-example/test.c:89
??
??:0

$ make CC=clang info clean check
clang -v
clang version 3.1 (branches/release_31)
Target: i386-pc-cygwin
Thread model: posix
rm -f test-cygwin_stackdump.exe
rm -f test-cygwin_stackdump.exe.stackdump
rm -f test-unwind.exe
clang -DUSE_CYGWIN_STACKDUMP -g -O0 -std=gnu99 -Wall -funwind-tables -o test-cygwin_stackdump.exe test.c
clang -DUSE_UNWIND -g -O0 -std=gnu99 -Wall -funwind-tables -o test-unwind.exe test.c
./test-cygwin_stackdump.exe
Stack trace:
Frame     Function  Args
0028ABDC  00401258 (0028ABEC, 00401238, 0028ABF4, 00401228)
0028ABE4  00401248 (0028ABF4, 00401228, 0028ABFC, 00401218)
0028ABEC  00401238 (0028ABFC, 00401218, 0028AC04, 00401208)
0028ABF4  00401228 (0028AC04, 00401208, 0028AC0C, 004011F8)
0028ABFC  00401218 (0028AC0C, 004011F8, 0028AC14, 004011E8)
0028AC04  00401208 (0028AC14, 004011E8, 0028AC1C, 004011D8)
0028AC0C  004011F8 (0028AC1C, 004011D8, 0028AC24, 004011C8)
0028AC14  004011E8 (0028AC24, 004011C8, 0028AC38, 004011B3)
0028AC1C  004011D8 (0028AC38, 004011B3, 0028AC5C, 00000001)
0028AC24  004011C8 (0028AC5C, 00000001, 00000000, 0028ACF8)
0028AC38  004011B3 (00000001, 0028AC5C, 80010100, 61007F58)
0028ACF8  61007FB5 (00000000, 0028CD84, 61007120, 00000000)
End of stack trace
./test-cygwin_stackdump.exe 2>&1 | awk '/^[0-9]/ { print $2 }' | addr2line -f -e ./test-cygwin_stackdump.exe
BFD: Dwarf Error: Offset (4263950) greater than or equal to .debug_str size (1399).
BFD: Dwarf Error: Offset (4263990) greater than or equal to .debug_str size (1399).
BFD: Dwarf Error: Offset (4263997) greater than or equal to .debug_str size (1399).
BFD: Dwarf Error: Offset (4260301) greater than or equal to .debug_line size (3462).
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
BFD: Dwarf Error: Offset (4260301) greater than or equal to .debug_line size (3420).
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
