#if defined(USE_CYGWIN_STACKDUMP)
	#include <assert.h>
	#include <stdio.h>
	#include <stdbool.h>
	#include <stdlib.h>
	#include <string.h>
	#include <unistd.h>
	#include <sys/types.h>
	#include <sys/wait.h>
	extern void cygwin_stackdump();
	static void show_stackdump() {
		// NOTE: fopen() stackdump file immediately after cygwin_stackdump() calling
		//       in signle process will fail, so use fork() and waitpid().
		pid_t pid = fork();
		if (pid == 0) {
			cygwin_stackdump();
			return;
		} else if (pid > 0){
			int status;
			if (waitpid(pid, &status, 0) != pid) {
				assert(false);
			}
			if (WEXITSTATUS(status) != 0) {
				assert(false);
			}
			char *fname = (char *)realloc(
				strdup(__FILE__),
				strlen(__FILE__)
				- strlen(".c")
				+ strlen("-cygwin_stackdump.exe.stackdump")
				+ 1
			);
			strcpy(&fname[strlen(__FILE__) - strlen(".c")], "-cygwin_stackdump.exe.stackdump");
			FILE *fp = fopen(fname, "r");
			assert(fp != NULL);
			size_t size;
			char buf[1024];
			while ((size = fread(buf, sizeof(char), 1024, fp)) > 0)
				fwrite(buf, sizeof(char), size, stderr);
			fclose(fp);
			remove(fname);
			free(fname);
			return;
		} else {
			assert(false);
		}
	}
#elif defined(USE_UNWIND)
	// FIXME: cygwin has clang-3.1 as most recent version.
	//        w/ 3.1, only _Unwind_GetIP() and _Unwind_Backtrace() are supported on !arm.
	//        as upstream version, see http://clang.llvm.org/doxygen/unwind_8h.html.
	#include <stdio.h>
	#include <inttypes.h>
	#include <unwind.h>
	_Unwind_Reason_Code trace_fn(struct _Unwind_Context *ctx, void *d) {
		int *depth = (int *)d;
		fprintf(stderr, "#%d: %08" PRIxPTR "\n", *depth, _Unwind_GetIP(ctx));
		(*depth)++;
		return _URC_NO_REASON;
	}
	static void show_stackdump() {
		int depth = 0;
		_Unwind_Backtrace(&trace_fn, &depth);
	}
#endif

static void f10() {
	show_stackdump();
	return;
}

#define fdecl(a, b)      \
	static void f##a() { \
		return f##b();   \
	}
fdecl(9, 10)
fdecl(8,  9)
fdecl(7,  8)
fdecl(6,  7)
fdecl(5,  6)
fdecl(4,  5)
fdecl(3,  4)
fdecl(2,  3)
fdecl(1,  2)
fdecl(0,  1)

int main(int argc, char *argv[]) {
	f0();
	return 0;
}
