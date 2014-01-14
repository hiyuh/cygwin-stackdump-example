#if defined(USE_CYGWIN_STACKDUMP)
	#include <assert.h>
	#include <stdio.h>
	#include <stdbool.h>
	#include <stdlib.h>
	#include <string.h>
	#include <unistd.h>
	#include <sys/types.h>
	#include <sys/wait.h>
	#include <errno.h>
	extern void cygwin_stackdump();
	// FIXME: Use __argv?
	static char *ename;
	static void show_stackdump() {
		#if defined(USE_FORK_WAITPID)
			// NOTE: fopen() stackdump file immediately after cygwin_stackdump() calling
			//       in signle process will fail, so use fork() and waitpid().
			pid_t pid = fork();
			if (pid == 0) {
				cygwin_stackdump();
				return;
			} else if (pid > 0) {
				int status;
				if (waitpid(pid, &status, 0) != pid) {
					assert(false);
				}
				if (WEXITSTATUS(status) != 0) {
					assert(false);
				}
				char *fname = (char *)malloc(strlen(ename) + sizeof (".exe.stackdump"));
				sprintf(fname, "%s.exe.stackdump", ename);
				FILE *fp = fopen(fname, "r");
				if (fp == NULL) {
					perror("fopen");
					assert(false);
				}
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
		#else
			cygwin_stackdump();
			char *fname = (char *)malloc(strlen(ename) + sizeof (".exe.stackdump"));
			sprintf(fname, "%s.exe.stackdump", ename);
			FILE *fp = fopen(fname, "r");
			if (fp == NULL) {
				perror("fopen");
				assert(false);
			}
			size_t size;
			char buf[1024];
			while ((size = fread(buf, sizeof(char), 1024, fp)) > 0)
				fwrite(buf, sizeof(char), size, stderr);
			fclose(fp);
			remove(fname);
			free(fname);
			return;
		#endif
	}
#elif defined(USE_UNWIND)
	// FIXME: cygwin has clang-3.1 as most recent version.
	//        w/ 3.1, only _Unwind_GetIP() and _Unwind_Backtrace() are supported on !arm.
	//        as upstream version, see http://clang.llvm.org/doxygen/unwind_8h.html.
	#include <stdio.h>
	#include <stdlib.h>
	#include <inttypes.h>
	#include <unwind.h>
	typedef struct trace_arg {
		size_t depth;
		uintptr_t *ip;
	} trace_arg_t;
	static _Unwind_Reason_Code trace_fn(struct _Unwind_Context *ctx, void *a) {
		trace_arg_t *arg = (trace_arg_t *)a;
		arg->depth++;
		arg->ip = (uintptr_t *)realloc(arg->ip, sizeof(uintptr_t) * arg->depth);
		arg->ip[arg->depth - 1] = _Unwind_GetIP(ctx);
		return _URC_NO_REASON;
	}
	static void show_stackdump() {
		trace_arg_t arg = {
			.depth = 0,
			.ip = NULL
		};
		_Unwind_Backtrace(&trace_fn, &arg);
		for (size_t d = 0; d < arg.depth; d++) {
			fprintf(stderr, "#%d: %08" PRIxPTR "\n", arg.depth, arg.ip[d]);
		}
		free(arg.ip);
		return;
	}
#endif

#if defined(USE_SIGNAL)
	#include <assert.h>
	#include <signal.h>
#endif

static void f10() {
	#if defined(USE_SIGNAL)
		if (raise(SIGSEGV) != 0) {
			perror("raise");
		}
	#else
		show_stackdump();
	#endif
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

#if defined(USE_SIGNAL)
static void signal_handler(int sig, siginfo_t *info, void *secret) {
	// FIXME: Following typedef for ucontext_t is plain incorrect.
	//        cygwin signal does not support SA_SIGINFO properly ATM.
	//        So the 3rd argument of signal handler is NULL even SA_SIGINFO is set.
	//typedef struct ucontext ucontext_t;
	//#if defined(__x86_64__)
	//	#define PC_FROM_UCONTEXT rip
	//#else
	//	#define PC_FROM_UCONTEXT eip
	//#endif
	//ucontext_t *uc = (ucontext_t*)secret;
	//uintptr_t ip = uc->PC_FROM_UCONTEXT;
	assert(secret == NULL);
	show_stackdump();
	return;
}

static void register_signal_handler(void) {
	struct sigaction sa;
	sa.sa_sigaction = (void*)signal_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART | SA_SIGINFO;
	sigaction(SIGSEGV, &sa, NULL);
	return;
}
#endif

int main(int argc, char *argv[]) {
	#if defined(USE_SIGNAL)
	register_signal_handler();
	#endif
	#if defined(USE_CYGWIN_STACKDUMP)
		ename = argv[0];
	#endif
	f0();
	return 0;
}
