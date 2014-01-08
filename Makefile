# NOTE: -funwind-tables is required for clang-3.1, gcc-4.8.2 does not want.
#       -fasynchronous-unwind-tables is required?
#       -fexceptions is required?
#       -g is required to work addr2line properly.
CFLAGS = -g -O0 -std=gnu99 -Wall -funwind-tables

all: check

test-cygwin_stackdump.exe: test.c
	$(CC) -DUSE_CYGWIN_STACKDUMP $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LIBS)

test-cygwin_stackdump-fork-waitpid.exe: test.c
	$(CC) -DUSE_CYGWIN_STACKDUMP -DUSE_FORK_WAITPID $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LIBS)

test-unwind.exe: test.c
	$(CC) -DUSE_UNWIND $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LIBS)

.PHONY: info check clean

info:
	$(CC) -v

check:                        \
	test-cygwin_stackdump.exe \
	test-cygwin_stackdump-fork-waitpid.exe \
	test-unwind.exe
	./test-cygwin_stackdump.exe || true
	./test-cygwin_stackdump.exe 2>&1 | awk '/^[0-9]/ { print $$2 }' | addr2line -f -e ./test-cygwin_stackdump.exe || true
	./test-cygwin_stackdump-fork-waitpid.exe
	./test-cygwin_stackdump-fork-waitpid.exe 2>&1 | awk '/^[0-9]/ { print $$2 }' | addr2line -f -e ./test-cygwin_stackdump-fork-waitpid.exe
	./test-unwind.exe
	./test-unwind.exe           2>&1 | awk          '{ print $$2 }' | addr2line -f -e ./test-unwind.exe

clean:
	rm -f test-cygwin_stackdump.exe
	rm -f test-cygwin_stackdump.exe.stackdump
	rm -f test-cygwin_stackdump-fork-waitpid.exe
	rm -f test-cygwin_stackdump-fork-waitpid.exe.stackdump
	rm -f test-unwind.exe
