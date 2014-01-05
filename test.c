extern void cygwin_stackdump();

static void f10() {
	cygwin_stackdump();
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
