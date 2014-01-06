tasting `cygwin_stackdump()`.

result:

* `cygwin_stackdump()` creates a file named executable's plus suffixed `.stackdump`.
* `cygwin_stackdump()` writes simple stack trace which are constructed by frame,   
  function and args in hex notation.

```
$ uname -a
CYGWIN_NT-5.1 jabberwocky 1.7.27(0.271/5/3) 2013-12-09 11:57 i686 Cygwin

$ ls
test.c

$ make CFLAGS="-O0" test
cc -O0    test.c   -o test

$ ls
test.c  test.exe

$ ./test.exe
$ ls
test.c  test.exe  test.exe.stackdump
 
$ cat test.exe.stackdump
Stack trace:
Frame     Function  Args
0022ABD8  004011B7 (00000000, 00000000, 0022ABF8, 004011D3)
0022ABE8  004011C5 (004017E0, 004017E0, 0022AC08, 004011E1)
0022ABF8  004011D3 (611882B0, 7FFDFC00, 0022AC18, 004011EF)
0022AC08  004011E1 (7C80B760, 00000000, 0022AC28, 004011FD)
0022AC18  004011EF (00000000, 610063A0, 0022AC38, 0040120B)
0022AC28  004011FD (004017E4, 004017E0, 0022AC48, 00401219)
0022AC38  0040120B (610063A0, 6119EF5B, 0022AC58, 00401227)
0022AC48  00401219 (0000002F, 0022AC8C, 0022AC68, 0040123A)
0022AC58  00401227 (00000000, 0022CDB4, 0022AD28, 61007FB5)
0022AC68  0040123A (00000001, 0022AC8C, 20010100, 61007F58)
0022AD28  61007FB5 (00000000, 0022CDB4, 61007120, 00000000)
End of stack trace
```
